#ifndef LIGHTHELPER_INC_H
#define LIGHTHELPER_INC_H

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
    ambient  = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse  = float4(0.0f, 0.0f, 0.0f, 0.0f);
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
    ambient  = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse  = float4(0.0f, 0.0f, 0.0f, 0.0f);
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
    ambient  = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse  = float4(0.0f, 0.0f, 0.0f, 0.0f);
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
    
    float3 bumpedNormalW = mul(TBN, normalT);

    return bumpedNormalW;
}

#endif // LIGHTHELPER_INC_H