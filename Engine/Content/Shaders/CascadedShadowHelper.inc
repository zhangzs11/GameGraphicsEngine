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

Texture2DArray g_ShadowMap : register(t10);
SamplerComparisonState g_SamShadow : register(s10);

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
    
}

//--------------------------------------------------------------------------------------
// Computes the blending amount and blending region between two cascades
//--------------------------------------------------------------------------------------
void CalculateBlendAmountForMap(float4 shadowMapTexCoord,
                                inout float currentPixelsBlendBandLocation,
                                inout float blendBetweenCascadesAmount)
{
    
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
    
    return percentLit;
}

#endif // CASCADESHADOWHELPER_INC_H