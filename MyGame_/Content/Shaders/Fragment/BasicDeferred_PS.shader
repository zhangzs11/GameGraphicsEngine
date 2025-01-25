struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular; // w = SpecPower
    float4 reflect;
};
struct PointLight
{
    float3 posV;
    float attenuationBegin;
    float3 color;
    float attenuationEnd;
};
float linstep(float min_value, float max_value, float value)
{
    return saturate((value - min_value) / (max_value - min_value));
}
// Constant Buffers
//=================
cbuffer g_constantBuffer_frame : register( b0 )
{
	// Main Rendering
	// --------------------------------------
	float4x4 g_transform_worldToCamera;
	float4x4 g_transform_cameraToProjected;

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	float2 g_padding1;

    float4 g_CameraNearFar;
};

cbuffer g_constantBuffer_drawCall : register( b2 )
{
    float4x4 g_transform_localToWorld;
	float4x4 g_transform_localToWorld_Inv_Transpose;

    Material g_Material;
};

// 3 GBuffer and a depth buffer(t4) t1-t4
// Texture2D<float4> g_GBufferTextures[4] : register(t1);

Texture2D<float4> g_GBufferTextures_01 : register(t1);
Texture2D<float4> g_GBufferTextures_02 : register(t2);
Texture2D<float4> g_GBufferTextures_03 : register(t3);
Texture2D<float4> g_GBufferTextures_04 : register(t4);

// Many Dynamic Point Light
StructuredBuffer<PointLight> g_Light : register(t5);

struct SurfaceData
{
    float3 posV;
    float3 posV_DX;
    float3 posV_DY;
    float3 normalV;
    float4 albedo;
    float specularAmount;
    float specularPower;
};

struct GBuffer
{
    float4 normal_specular;
    float4 albedo;
    float2 posZGrad;  // ( d(x+1,y)-d(x,y), d(x,y+1)-d(x,y) )
};

float3 DecodeSphereMap(float2 encoded)
{
    float4 nn = float4(encoded, 1, -1);
    float l = dot(nn.xyz, -nn.xyw);
    nn.z = l;
    nn.xy *= sqrt(l);
    return nn.xyz * 2 + float3(0, 0, -1);
}

float3 ComputePositionViewFromZ(float2 posNdc, float viewSpaceZ)
{
    float2 screenSpaceRay = float2(posNdc.x / - g_transform_cameraToProjected._m00,
                                   posNdc.y / - g_transform_cameraToProjected._m11);
    
    float3 posV;
    posV.z = viewSpaceZ;
    posV.xy = screenSpaceRay.xy * posV.z;
    
    return posV;
}

void AccumulatePhong(float3 normal, float3 lightDir, float3 viewDir, float3 lightContrib, float specularPower,
                     inout float3 litDiffuse, inout float3 litSpecular)
{
    float NdotL = dot(normal, lightDir);
    [flatten]
    if (NdotL > 0.0f)
    {
        float3 r = reflect(lightDir, normal);
        float RdotV = max(0.0f, dot(r, viewDir));
        float specular = pow(RdotV, specularPower);
        
        litDiffuse += lightContrib * NdotL;
        litSpecular += lightContrib * specular;
    }
}

void AccumulateDiffuseSpecular(SurfaceData surface, PointLight light,
                               inout float3 litDiffuse, inout float3 litSpecular)
{
    float3 dirToLight = light.posV - surface.posV;
    float distToLight = length(dirToLight);
    
    [branch]
    if (distToLight < light.attenuationEnd)
    {
        float attenuation = linstep(light.attenuationEnd, light.attenuationBegin, distToLight);
        dirToLight *= rcp(distToLight);
        AccumulatePhong(surface.normalV, dirToLight, normalize(surface.posV),
            attenuation * light.color, surface.specularPower, litDiffuse, litSpecular);
    }
}

SurfaceData ComputeSurfaceDataFromGBufferSample(uint2 posViewport)
{
    GBuffer rawData;
    rawData.normal_specular = g_GBufferTextures_01.Load(int3(posViewport.xy, 0)).xyzw;
    rawData.albedo = g_GBufferTextures_02.Load(int3(posViewport.xy, 0)).xyzw;
    rawData.posZGrad = g_GBufferTextures_03.Load(int3(posViewport.xy, 0)).xy;
    float zBuffer = g_GBufferTextures_04.Load(int3(posViewport.xy, 0)).x;


    float2 gbufferDim;
    g_GBufferTextures_01.GetDimensions(gbufferDim.x, gbufferDim.y);

    float2 screenPixelOffset = float2(2.0f, -2.0f) / gbufferDim;
    float2 posNdc = (float2(posViewport.xy) + 0.5f) * screenPixelOffset.xy + float2(-1.0f, 1.0f);
    float2 posNdcX = posNdc + float2(screenPixelOffset.x, 0.0f);
    float2 posNdcY = posNdc + float2(0.0f, screenPixelOffset.y);

    SurfaceData data;

    float viewSpaceZ = g_transform_cameraToProjected._m23 / (-zBuffer - g_transform_cameraToProjected._m22); // m32 maybe need to m23

    data.posV = ComputePositionViewFromZ(posNdc, viewSpaceZ);  // Position need to think more
    data.posV_DX = ComputePositionViewFromZ(posNdcX, viewSpaceZ + rawData.posZGrad.x) - data.posV;
    data.posV_DY = ComputePositionViewFromZ(posNdcY, viewSpaceZ + rawData.posZGrad.y) - data.posV;

    data.normalV = DecodeSphereMap(rawData.normal_specular.xy);
    data.albedo = rawData.albedo;

    data.specularAmount = rawData.normal_specular.z;
    data.specularPower = rawData.normal_specular.w;

    return data;
}

void AccumulateColor(SurfaceData surface, PointLight light,
                     inout float3 litColor)
{
    float3 dirToLight = light.posV - surface.posV;
    float distToLight = length(dirToLight);
    
    [branch]
    if (distToLight < light.attenuationEnd)
    {
        float attenuation = linstep(light.attenuationEnd, light.attenuationBegin, distToLight);
        dirToLight *= rcp(distToLight);
        
        float3 litDiffuse = float3(0.0f, 0.0f, 0.0f);
        float3 litSpecular = float3(0.0f, 0.0f, 0.0f);
        AccumulatePhong(surface.normalV, dirToLight, normalize(surface.posV),
            attenuation * light.color, surface.specularPower, litDiffuse, litSpecular);
        litColor += surface.albedo.rgb * (litDiffuse + surface.specularAmount * litSpecular);
    }
}

// Entry Point
//============

void main(
	// Input
	//======
	in const float4 i_fragmentPosition : SV_POSITION,

	// Output
	//=======
	out float4 o_color : SV_TARGET
)

{
	uint totalLights, dummy;
    g_Light.GetDimensions(totalLights, dummy);
    
    float3 lit = float3(0.0f, 0.0f, 0.0f);

    // [branch]
    // if (g_VisualizeLightCount)
    // {
    //     lit = (float(totalLights) * rcp(255.0f)).xxx;
    // }
    // else
        SurfaceData surface = ComputeSurfaceDataFromGBufferSample(uint2(i_fragmentPosition.xy));

        if (surface.posV.z < g_CameraNearFar.y)
        {
            for (uint lightIndex = 0; lightIndex < totalLights; ++lightIndex)
            {
                PointLight light = g_Light[lightIndex];
                AccumulateColor(surface, light, lit);
            }
        }

         // lit = surface.posV;

     o_color = float4(lit, 1.0f);


    // o_color = g_GBufferTextures_02.Load(int3(i_fragmentPosition.xy, 0));

    // TEST TEXTURE INPUT
    //
    // uint2 pixelCoords = uint2(i_fragmentPosition.xy);

    // uint2 gbufferDim;
    // g_GBufferTextures_01.GetDimensions(gbufferDim.x, gbufferDim.y);

    // o_color = float4(0.0f, 0.0f, 0.0f, 1.0f);

    // if (pixelCoords.x < gbufferDim.x / 2 && pixelCoords.y < gbufferDim.y / 2)
    // {
    //     o_color = g_GBufferTextures_01.Load(int3(pixelCoords.xy * 2, 0));
    // }
    // else if (pixelCoords.x >= gbufferDim.x / 2 && pixelCoords.y < gbufferDim.y / 2)
    // {
    //     o_color = g_GBufferTextures_02.Load(int3((pixelCoords.x - gbufferDim.x / 2) * 2, pixelCoords.y * 2, 0));
    // }
    // else if (pixelCoords.x < gbufferDim.x / 2 && pixelCoords.y >= gbufferDim.y / 2)
    // {
    //     o_color = g_GBufferTextures_03.Load(int3(pixelCoords.x * 2, (pixelCoords.y - gbufferDim.y / 2) * 2, 0));
    // }
    // else if (pixelCoords.x >= gbufferDim.x / 2 && pixelCoords.y >= gbufferDim.y / 2)
    // {
    //     float depth = g_GBufferTextures_04.Load(int3
    //     (
    //     (pixelCoords.x - gbufferDim.x / 2) * 2,
    //     (pixelCoords.y - gbufferDim.y / 2) * 2,
    //     0
    //     )
    //     ).r;
    //     o_color = (depth == 1.0) ? float4(1.0, 1.0, 1.0, 1.0) : float4(0.0, 0.0, 0.0, 1.0);
    // }
}