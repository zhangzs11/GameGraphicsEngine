#ifndef CASCADESHADOWHELPER_INC_H
#define CASCADESHADOWHELPER_INC_H

// Use partial derivatives to map texels in the shadow map to the view-space plane
// of the primitive being rendered. This depth will be used for comparison
// to reduce shadow aliasing. This technique is expensive and assumes the object
// consists mostly of flat surfaces.
#ifndef USE_DERIVATIVES_FOR_DEPTH_OFFSET_FLAG
#define USE_DERIVATIVES_FOR_DEPTH_OFFSET_FLAG 0
#endif

// Allows blending shadow values between different cascades. 
// Most effective when shadow maps are small and artifacts are visible
// between two cascades.
#ifndef BLEND_BETWEEN_CASCADE_LAYERS_FLAG
#define BLEND_BETWEEN_CASCADE_LAYERS_FLAG 0
#endif

// There are two methods to select the appropriate cascade for the current pixel fragment:
// Interval-based Selection compares the depth partition of the view frustum
// with the pixel fragment depth.
//
// Map-based Selection finds the smallest cascade where the texture coordinates
// are within the shadow map's range.
#ifndef SELECT_CASCADE_BY_INTERVAL_FLAG
#define SELECT_CASCADE_BY_INTERVAL_FLAG 0
#endif

// Number of cascades
#ifndef CASCADE_COUNT_FLAG
#define CASCADE_COUNT_FLAG 3
#endif

// In most cases, using 3-4 cascades and enabling BLEND_BETWEEN_CASCADE_LAYERS_FLAG
// is sufficient for low-end PCs. High-end PCs can handle larger shadows
// and wider blending zones. When using larger PCF kernels, high-end PCs
// can also use derivative-based depth offsets.

//Texture2DArray g_ShadowMap : register(t10);
//SamplerComparisonState g_SamShadow : register(s10);

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
cbuffer g_constantBuffer_frame : register(b0)
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
    
	//// Shadow Map Rendering
	//// ---------------------------------------
    //   float4x4 g_transform_worldToShadowMapCamera;
    //   float4x4 g_transform_cameraToShadowMapProjected;
	
    
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
cbuffer g_constantBuffer_drawCall : register(b2)
{
    float4x4 g_transform_localToWorld;
    float4x4 g_transform_localToWorld_Inv_Transpose;
	
    Material g_Material;
};

// Texture2DArray g_ShadowMap : register(t2); // Shadow map
// SamplerComparisonState g_SamShadow : register(s1);

static const float4 s_CascadeColorsMultiplier[8] =
{
    float4(1.5f, 0.0f, 0.0f, 1.0f),
    float4(0.0f, 1.5f, 0.0f, 1.0f),
    float4(0.0f, 0.0f, 5.5f, 1.0f),
    float4(1.5f, 0.0f, 5.5f, 1.0f),
    float4(1.5f, 1.5f, 0.0f, 1.0f),
    float4(1.0f, 1.0f, 1.0f, 1.0f),
    float4(0.0f, 1.0f, 5.5f, 1.0f),
    float4(0.5f, 3.5f, 0.75f, 1.0f)
};

//--------------------------------------------------------------------------------------
// Calculates the corresponding light-space texel deltas for texels in shadow space
//--------------------------------------------------------------------------------------
void CalculateRightAndUpTexelDepthDeltas(float3 shadowTexDDX, float3 shadowTexDDY,
                                         out float upTextDepthWeight,
                                         out float rightTextDepthWeight)
{
    float2x2 matScreenToShadow = float2x2(shadowTexDDX.xy, shadowTexDDY.xy);
    float det = determinant(matScreenToShadow);
    float invDet = 1.0f / det;
    float2x2 matShadowToScreen = float2x2(
        matScreenToShadow._22 * invDet, matScreenToShadow._12 * -invDet,
        matScreenToShadow._21 * -invDet, matScreenToShadow._11 * invDet);
    
    float2 rightShadowTexelLocation = float2(g_TexelSize_shadowMap, 0.0f);
    float2 upShadowTexelLocation = float2(0.0f, g_TexelSize_shadowMap);
    
    float2 rightTexelDepthRatio = mul(rightShadowTexelLocation, matShadowToScreen);
    float2 upTexelDepthRatio = mul(upShadowTexelLocation, matShadowToScreen);
    
    upTextDepthWeight =
        upTexelDepthRatio.x * shadowTexDDX.z 
        + upTexelDepthRatio.y * shadowTexDDY.z;
    rightTextDepthWeight =
        rightTexelDepthRatio.x * shadowTexDDX.z 
        + rightTexelDepthRatio.y * shadowTexDDY.z;
}

//--------------------------------------------------------------------------------------
// Samples the depth map using PCF and returns the percentage of lit shading
//--------------------------------------------------------------------------------------
float CalculatePCFPercentLit(int currentCascadeIndex,
                             float4 shadowTexCoord,
                             float rightTexelDepthDelta,
                             float upTexelDepthDelta,
                             float blurSize)
{
    float percentLit = 0.0f;
    
    for (int x = g_PCFBlurForLoopStart; x < g_PCFBlurForLoopEnd; ++x)
    {
        for (int y = g_PCFBlurForLoopStart; y < g_PCFBlurForLoopEnd; ++y)
        {
            float depthCmp = shadowTexCoord.z;

            depthCmp -= g_ShadowBias;
            if (USE_DERIVATIVES_FOR_DEPTH_OFFSET_FLAG)
            {
                depthCmp += rightTexelDepthDelta * (float) x + upTexelDepthDelta * (float) y;
            }

            //percentLit += g_ShadowMap.SampleCmpLevelZero(g_SamShadow,
            //    float3(
            //        shadowTexCoord.x + (float) x * g_TexelSize_shadowMap,
            //        shadowTexCoord.y + (float) y * g_TexelSize_shadowMap,
            //        (float) currentCascadeIndex
            //    ),
            //    depthCmp);
            

            percentLit += g_ShadowMap.SampleCmpLevelZero(g_SamShadow,
            float3(shadowTexCoord.xy, (float) currentCascadeIndex), depthCmp).r;


            
            //float sampledDepth = g_ShadowMap.SampleLevel(g_SamShadow,
            //    float3(shadowTexCoord.xy, currentCascadeIndex), 0.0f).r;

            //percentLit += (depthCmp <= sampledDepth) ? 1.0f : 0.0f;

        }
    }
    
    percentLit /= blurSize;
    return percentLit;
}

//--------------------------------------------------------------------------------------
// Computes the blending amount and blending region between two cascades
//--------------------------------------------------------------------------------------
void CalculateBlendAmountForInterval(int currentCascadeIndex,
                                     inout float pixelDepth,
                                     inout float currentPixelsBlendBandLocation,
                                     out float blendBetweenCascadesAmount)
{
    float blendInterval = g_CascadeFrustumsEyeSpaceDepthsFloat4[currentCascadeIndex].x;
    
    if (currentCascadeIndex > 0)
    {
        int blendIntervalbelowIndex = currentCascadeIndex - 1;
        pixelDepth -= g_CascadeFrustumsEyeSpaceDepthsFloat4[blendIntervalbelowIndex].x;
        blendInterval -= g_CascadeFrustumsEyeSpaceDepthsFloat4[blendIntervalbelowIndex].x;
    }
    
    currentPixelsBlendBandLocation = 1.0f - pixelDepth / blendInterval;

    blendBetweenCascadesAmount = currentPixelsBlendBandLocation / g_CascadeBlendArea;
}

//--------------------------------------------------------------------------------------
// Computes the blending amount and blending region between two cascades
//--------------------------------------------------------------------------------------
void CalculateBlendAmountForMap(float4 shadowMapTexCoord,
                                inout float currentPixelsBlendBandLocation,
                                inout float blendBetweenCascadesAmount)
{
    float2 distanceToOne = float2(1.0f - shadowMapTexCoord.x, 1.0f - shadowMapTexCoord.y);
    currentPixelsBlendBandLocation = min(shadowMapTexCoord.x, shadowMapTexCoord.y);
    float currentPixelsBlendBandLocation2 = min(distanceToOne.x, distanceToOne.y);
    currentPixelsBlendBandLocation =
        min(currentPixelsBlendBandLocation, currentPixelsBlendBandLocation2);
    
    blendBetweenCascadesAmount = currentPixelsBlendBandLocation / g_CascadeBlendArea;
}

//--------------------------------------------------------------------------------------
// GetCascadeColorMultipler
//--------------------------------------------------------------------------------------
float4 GetCascadeColorMultipler(int currentCascadeIndex,
                                int nextCascadeIndex,
                                float blendBetweenCascadesAmount)
{
    return lerp(s_CascadeColorsMultiplier[nextCascadeIndex],
                s_CascadeColorsMultiplier[currentCascadeIndex],
                blendBetweenCascadesAmount);
}

//--------------------------------------------------------------------------------------
// MAIN
// CalculateCascadedShadow
//--------------------------------------------------------------------------------------
float CalculateCascadedShadow(float4 shadowMapTexCoordViewSpace,
                              float currentPixelDepth,
                              out int currentCascadeIndex,
                              out int nextCascadeIndex,
                              out float blendBetweenCascadesAmount)
{
    float percentLit = 0.0f;
    float percentLit_blend = 0.0f;
    
    float4 shadowMapTexCoord = 0.0f;
    float4 shadowMapTexCoord_blend = 0.0f;
    
    float4 visualizeCascadeColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    float upTextDepthWeight = 0;
    float rightTextDepthWeight = 0;
    float upTextDepthWeight_blend = 0;
    float rightTextDepthWeight_blend = 0;
    
    float blurSize = g_PCFBlurForLoopEnd - g_PCFBlurForLoopStart;
    blurSize *= blurSize;
    
    int cascadeFound = 0;
    nextCascadeIndex = 1;
    
    
    //
    // SELECT the cascade Index, and then transform the shadow map coordinate
    //
    
    // Interval-Based Selection
    if (SELECT_CASCADE_BY_INTERVAL_FLAG)
    {
        currentCascadeIndex = 0;
        //                               Depth
        // /-+-------/----------------/----+-------/----------/
        // 0 N     F[0]     ...      F[i]        F[i+1] ...   F
        // Depth > F[i] to F[0] => index = i+1
        if (CASCADE_COUNT_FLAG > 1)
        {
            float4 currentPixelDepthVec = currentPixelDepth;
            float4 cmpVec1 = (currentPixelDepthVec > g_CascadeFrustumsEyeSpaceDepthsFloat[0]);
            float4 cmpVec2 = (currentPixelDepthVec > g_CascadeFrustumsEyeSpaceDepthsFloat[1]);
            float index = dot(float4(CASCADE_COUNT_FLAG > 0,
                                     CASCADE_COUNT_FLAG > 1,
                                     CASCADE_COUNT_FLAG > 2,
                                     CASCADE_COUNT_FLAG > 3),
                              cmpVec1) +
                          dot(float4(CASCADE_COUNT_FLAG > 4,
                                     CASCADE_COUNT_FLAG > 5,
                                     CASCADE_COUNT_FLAG > 6,
                                     CASCADE_COUNT_FLAG > 7),
                              cmpVec2);
            index = min(index, CASCADE_COUNT_FLAG - 1);
            currentCascadeIndex = (int) index;
        }
        
        // Calculate the shadow map coordinate
        shadowMapTexCoord = shadowMapTexCoordViewSpace * g_CascadeScale[currentCascadeIndex] + g_CascadeOffset[currentCascadeIndex];
    }
    
    // Map-Based Selection
    if (!SELECT_CASCADE_BY_INTERVAL_FLAG)
    {
        currentCascadeIndex = 0;
        if (CASCADE_COUNT_FLAG == 1)
        {
            shadowMapTexCoord = shadowMapTexCoordViewSpace * g_CascadeScale[0] + g_CascadeOffset[0];
        }
        if (CASCADE_COUNT_FLAG > 1)
        {
            // research the nearset cascade, make the shadow Map texture coord in the boundary of coord
            // minBorder < tx, ty < maxBorder
            for (int cascadeIndex = 0; cascadeIndex < CASCADE_COUNT_FLAG && cascadeFound == 0; ++cascadeIndex)
            {
                shadowMapTexCoord = shadowMapTexCoordViewSpace * g_CascadeScale[cascadeIndex] + g_CascadeOffset[cascadeIndex];
                if (min(shadowMapTexCoord.x, shadowMapTexCoord.y) > g_MinBorderPadding
                    && max(shadowMapTexCoord.x, shadowMapTexCoord.y) < g_MaxBorderPadding)
                {
                    currentCascadeIndex = cascadeIndex;
                    cascadeFound = 1;
                }
            }
        }
    }
    
    //
    // Calcualte the PCF (Percentage Closer Filtering) of current Cascade
    //
    float3 shadowMapTexCoordDDX;
    float3 shadowMapTexCoordDDY;
    // These partial derivatives are used to calculate the depth variation in light space 
    // caused by neighboring texels in the projected texture space along different directions.
    if (USE_DERIVATIVES_FOR_DEPTH_OFFSET_FLAG)
    {
        // Calculate the rate of change in projected texture space for light space derivatives
        shadowMapTexCoordDDX = ddx(shadowMapTexCoordViewSpace).xyz;
        shadowMapTexCoordDDY = ddy(shadowMapTexCoordViewSpace).xyz;
        
        shadowMapTexCoordDDX *= g_CascadeScale[currentCascadeIndex].xyz;
        shadowMapTexCoordDDY *= g_CascadeScale[currentCascadeIndex].xyz;
        
        CalculateRightAndUpTexelDepthDeltas(shadowMapTexCoordDDX, shadowMapTexCoordDDY,
                                            upTextDepthWeight, rightTextDepthWeight);
    }
    
    visualizeCascadeColor = s_CascadeColorsMultiplier[currentCascadeIndex];
    
    percentLit = CalculatePCFPercentLit(currentCascadeIndex, shadowMapTexCoord,
                                        rightTextDepthWeight, upTextDepthWeight, blurSize);
    
    
    //
    // Blend between two Cascades
    //
    if (BLEND_BETWEEN_CASCADE_LAYERS_FLAG)
    {
        nextCascadeIndex = min(CASCADE_COUNT_FLAG - 1, currentCascadeIndex + 1);
    }
    
    blendBetweenCascadesAmount = 1.0f;
    float currentPixelsBlendBandLocation = 1.0f;
    if (SELECT_CASCADE_BY_INTERVAL_FLAG)
    {
        if (BLEND_BETWEEN_CASCADE_LAYERS_FLAG && CASCADE_COUNT_FLAG > 1)
        {
            CalculateBlendAmountForInterval(currentCascadeIndex, currentPixelDepth,
                currentPixelsBlendBandLocation, blendBetweenCascadesAmount);
        }
    }
    else
    {
        if (BLEND_BETWEEN_CASCADE_LAYERS_FLAG)
        {
            CalculateBlendAmountForMap(shadowMapTexCoord,
                currentPixelsBlendBandLocation, blendBetweenCascadesAmount);
        }
    }
    
    if (BLEND_BETWEEN_CASCADE_LAYERS_FLAG && CASCADE_COUNT_FLAG > 1)
    {
        if (currentPixelsBlendBandLocation < g_CascadeBlendArea)
        {
            // Calculate the projected texture coordinates for the next cascade
            shadowMapTexCoord_blend = shadowMapTexCoordViewSpace * g_CascadeScale[nextCascadeIndex] + g_CascadeOffset[nextCascadeIndex];
            
            // When blending between cascades, also perform calculations for the next cascade
            if (currentPixelsBlendBandLocation < g_CascadeBlendArea)
            {
                // The current pixel is within the blending region
                if (USE_DERIVATIVES_FOR_DEPTH_OFFSET_FLAG)
                {
                    CalculateRightAndUpTexelDepthDeltas(shadowMapTexCoordDDX, shadowMapTexCoordDDY,
                                                        upTextDepthWeight_blend, rightTextDepthWeight_blend);
                }
                percentLit_blend = CalculatePCFPercentLit(nextCascadeIndex, shadowMapTexCoord_blend,
                                                          rightTextDepthWeight_blend, upTextDepthWeight_blend, blurSize);
                // Blend the PCF results of the two cascades
                percentLit = lerp(percentLit_blend, percentLit, blendBetweenCascadesAmount);
            }
        }
    }
    
    return percentLit;
}

#endif // CASCADESHADOWHELPER_INC_H