/*
	This file should be #included by all shaders
*/

#ifndef CONSTANTBUFFERS_INC_H
#define CONSTANTBUFFERS_INC_H

struct DirectionalLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float3 direction;
    float pad;
    float3 position;
    float pad2;
};

struct PointLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 position;
    float range;

    float3 attenuation;
    float pad;
};

struct SpotLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 position;
    float range;

    float3 direction;
    float Spot;

    float3 attenuation;
    float pad;
};

struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular; // w = SpecPower
    float4 reflect;
};

void ComputeDirectionalLight(Material material, DirectionalLight light,
                             float3 hit_normal, float3 hit_toEye,
                             out float4 ambient,
                             out float4 diffuse,
                             out float4 specular)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    ambient = material.ambient * light.ambient;
    
    // Assuming light.direction is normal vector
    float diffuseFactor = dot(-light.direction, hit_normal); // hit point to light = -light.direction
    
    // flatten, avoid dynamic branch
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 reflect_light = reflect(light.direction, hit_normal);
        float specFactor = pow(max(dot(reflect_light, hit_toEye), 0.0f), material.specular.w);
        
        diffuse = diffuseFactor * material.diffuse * light.diffuse;
        specular = specFactor * material.specular * light.specular;
    }
}

void ComputePointLight(Material material, PointLight light,
                       float3 hit_position, float3 hit_normal, float3 hit_toEye,
                       out float4 ambient,
                       out float4 diffuse,
                       out float4 specular)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    ambient = material.ambient * light.ambient;
    
    float3 hit_toLightVec = light.position - hit_position;
    
    float hit_toLight_distance = length(hit_toLightVec);
    
    // Light range test
    if (hit_toLight_distance > light.range)
        return;
    
    // hit to light vector normal
    hit_toLightVec /= hit_toLight_distance;
    
    float diffuseFactor = dot(hit_toLightVec, hit_normal);
    
    // flatten, avoid dynamic branch
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 reflect_light = reflect(-hit_toLightVec, hit_normal);
        float specFactor = pow(max(dot(reflect_light, hit_toEye), 0.0f), material.specular.w);
        
        diffuse = diffuseFactor * material.diffuse * light.diffuse;
        specular = specFactor * material.specular * light.specular;
    }
    
    float attenuation = 1.0f / dot(light.attenuation, float3(1.0f, hit_toLight_distance, hit_toLight_distance * hit_toLight_distance));
    
    diffuse *= attenuation;
    specular *= attenuation;
}

void ComputeSpotLight(Material material, SpotLight light,
                      float3 hit_position, float3 hit_normal, float3 hit_toEye,
                      out float4 ambient,
                      out float4 diffuse,
                      out float4 specular)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    ambient = material.ambient * light.ambient;
    
    float3 hit_toLightVec = light.position - hit_position;
    
    float hit_toLight_distance = length(hit_toLightVec);
    
    // Light range test
    if (hit_toLight_distance > light.range)
        return;
    
    // hit to light vector normal
    hit_toLightVec /= hit_toLight_distance;
    
    float diffuseFactor = dot(hit_toLightVec, hit_normal);
    
    // flatten, avoid dynamic branch
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 reflect_light = reflect(-hit_toLightVec, hit_normal);
        float specFactor = pow(max(dot(reflect_light, hit_toEye), 0.0f), material.specular.w);
        
        diffuse = diffuseFactor * material.diffuse * light.diffuse;
        specular = specFactor * material.specular * light.specular;
    }
    
    float spot = pow(max(dot(-hit_toLightVec, light.direction), 0.0f), light.Spot);
    float attenuation = spot / dot(light.attenuation, float3(1.0f, hit_toLight_distance, hit_toLight_distance * hit_toLight_distance));
    
    ambient *= spot;
    diffuse *= attenuation;
    specular *= attenuation;
}

static const float SMAP_SIZE = 1024.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerComparisonState samShadow,
                       Texture2D shadowMap,
                       float4 shadowPosH,
                       float depthBias)
{
    float percentLit = 0.0f;
    
    shadowPosH.xyz /= shadowPosH.w;
    
    float depth = shadowPosH.z - depthBias;
    
    const float dx = SMAP_DX;
    
    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow,
            shadowPosH.xy, depth, int2(i % 3 - 1, i / 3 - 1)).r;
    }
    
    return percentLit /= 9.0f;
}

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 uintNormalW, float4 tangentW)
{
    float3 normalT = 2.0f * normalMapSample - 1.0f;

    float3 N = uintNormalW;
    float3 T = normalize(tangentW.xyz - dot(tangentW.xyz, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);
    float3x3 TBN_Transpose = transpose(TBN);

    float3 bumpedNormalW = mul(TBN_Transpose, normalT);
    // float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
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
	
    DirectionalLight g_DirLight;
    PointLight g_PointLight[10];
    SpotLight g_SpotLight[10];
	
    float3 g_EyePosW;
    float g_padding2;
	
    float4x4 g_ShadowView;
    float4 g_CascadeOffset[8];
    float4 g_CascadeScale[8];
    int g_VisualizeCascades;
    int g_PCFBlurForLoopStart; 
    int g_PCFBlurForLoopEnd;
    int g_Pad;

    float g_MinBorderPadding;
    float g_MaxBorderPadding;
    float g_ShadowBias;
    float g_CascadeBlendArea;
      
    float g_TexelSize_shadowMap;
    float3 g_padding3;
    
    float4 g_CascadeFrustumsEyeSpaceDepthsFloat;
    float4 g_CascadeFrustumsEyeSpaceDepthsFloat4[4];
	
    
	// FXAA
	// ---------------------------------------
    float2 g_TexelSize_Screen;
    
    // Controls sharpness level
    // 1.00 - Soft
    // 0.75 - Default filter value
    // 0.50 - Sharper, removes less subpixel aliasing
    // 0.25 - Almost off
    // 0.00 - Completely off
    float g_QualitySubPix;
    
    // Threshold control for required local contrast
    // 0.333 - Very low (faster)
    // 0.250 - Low quality
    // 0.166 - Default
    // 0.125 - High quality
    // 0.063 - Very high (slower)
    float g_QualityEdgeThreshold;
    
    // Threshold for ignoring dark areas
    // 0.0833 - Default
    // 0.0625 - Slightly faster
    // 0.0312 - Slower
    float g_QualityEdgeThresholdMin;

    float3 g_padding4;
	
};
cbuffer g_constantBuffer_drawCall : register( b2 )
{
    float4x4 g_transform_localToWorld;
	float4x4 g_transform_localToWorld_Inv_Transpose;
	
    Material g_Material;
};


#endif // CONSTANTBUFFERS_INC_H

