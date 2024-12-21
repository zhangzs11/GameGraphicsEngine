struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular; // w = SpecPower
    float4 reflect;
};

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

Texture2D g_DiffuseMap : register(t0);
SamplerState g_Sam : register(s0);

//--------------------------------------------------------------------------------------
// SurfaceData
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

struct VertexPosHVNormalVTex
{
    float4 posH;
    float3 posV;
    float3 normalV;
    float2 texCoord;
};

float3 ComputeFaceNormal(float3 pos)
{
    return cross(ddx_coarse(pos), ddy_coarse(pos));
}

SurfaceData ComputeSurfaceDataFromGeometry(VertexPosHVNormalVTex input)
{
    SurfaceData surface;
    surface.posV = input.posV;
    
    // right/down pixel position distance to current pixel
    surface.posV_DX = ddx_coarse(surface.posV);
    surface.posV_DY = ddy_coarse(surface.posV);
    
    // facr normal replace normal in mesh provide
    float3 faceNormal = ComputeFaceNormal(input.posV);
    // surface.normalV = normalize(g_FaceNormals ? faceNormal : input.normalV);
    surface.normalV = faceNormal;

    surface.albedo = g_DiffuseMap.Sample(g_Sam, input.texCoord);
    // surface.albedo.rgb = g_LightingOnly ? float3(1.0f, 1.0f, 1.0f) : surface.albedo.rgb;
    surface.albedo.rgb = surface.albedo.rgb;

    // Hard code set up
    surface.specularAmount = 0.9f;
    surface.specularPower = 25.0f;
    
    return surface;
}

//--------------------------------------------------------------------------------------
// GBuffer
struct GBuffer
{
    float4 normal_specular : SV_Target0;  // R16G16B16A16_FLOAT
    float4 albedo : SV_Target1;           // R8G8B8A8_UNORM
    float2 posZGrad : SV_Target2;         // R16G16_FLOAT // ( d(x+1,y)-d(x,y), d(x,y+1)-d(x,y) )
};

float2 EncodeSphereMap(float3 normal)
{
    return normalize(normal.xy) * (sqrt(-normal.z * 0.5f + 0.5f));
}

//--------------------------------------------------------------------------------------
// Entry Point
void main(

	// Input
	//======

	in const float4 i_fragmentPosition : SV_POSITION,
	// in const float3 i_fragmentPosition_world : POSITION,

	in const float3 i_fragmentPosition_view : POSITION,

	// const float4 i_fragmentColor : COLOR,
	in const float2 i_fragmentUV : TEXCOORD0,
	// in float3 i_fragmentNormal_world : NORMAL,

	in float3 i_fragmentNormal_view : NORMAL,

	// in const float4 i_fragmentTangent_world : TANGENT,
	// in const float4 i_fragmentShadowPosH : TEXCOORD1,
	// in const float i_fragmentDepthV : TEXCOORD2,

	// Output
	//=======

	out GBuffer outputGBuffer

)

{
    VertexPosHVNormalVTex inputGeometry;
    inputGeometry.posH = i_fragmentPosition;
    inputGeometry.posV = i_fragmentPosition_view;
    inputGeometry.normalV = i_fragmentNormal_view;
    inputGeometry.texCoord = i_fragmentUV;

    SurfaceData surface = ComputeSurfaceDataFromGeometry(inputGeometry);
    outputGBuffer.normal_specular = float4(EncodeSphereMap(surface.normalV),
                                           surface.specularAmount,
                                           surface.specularPower);
    outputGBuffer.albedo = surface.albedo;
    outputGBuffer.posZGrad = float2(ddx_coarse(surface.posV.z),
                                    ddy_coarse(surface.posV.z));
}