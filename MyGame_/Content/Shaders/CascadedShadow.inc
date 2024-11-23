/*
	This file should be #included by all shaders
*/

#ifndef CASCADEDSHADOE_INC_H
#define CASCADEDSHADOE_INC_H

// Constant Buffers
//=================
cbuffer g_constantBuffer_frame : register( b0 )
{
	
	// Cascaded Shadow
    // ---------------
    float4x4 g_ShadowView;
    float4 g_CascadeOffset[8]; // Translation values for the Shadow PT (Projection-Translation) matrix
    float4 g_CascadeScale[8];  // Scaling values for the Shadow PT matrix
    int g_VisualizeCascades;   // 1 to visualize cascades with different colors, 0 to render the scene
    int g_PCFBlurForLoopStart; // Loop start value for PCF kernel (e.g., -2 for a 5x5 kernel)
    int g_PCFBlurForLoopEnd;   // Loop end value for PCF kernel (e.g., 3 for a 5x5 kernel)
    int g_Pad;
    
    
    // For the map-based selection approach, this ensures pixels remain within valid ranges.
    // When no borders are present, Min and Max are set to 0 and 1, respectively.
    float g_MinBorderPadding; // (kernelSize / 2) / (float)shadowSize
    float g_MaxBorderPadding; // 1.0f - (kernelSize / 2) / (float)shadowSize
    float g_ShadowBias;       // Offset to handle shadow artifacts, especially amplified by PCF
    float g_CascadeBlendArea; // Blend region between cascades
    
    
    float g_TexelSize; // Size of a texel in the shadow map
    float3 g_LightDir; // Direction of the light source
    
    float4 g_CascadeFrustumsEyeSpaceDepthsFloat[2];  // Z-values of the far planes of the sub-frustums, separating cascades
    float4 g_CascadeFrustumsEyeSpaceDepthsFloat4[8]; // Stored in float4 for array traversal (yzw components are unused)
};

#endif // CASCADEDSHADOE_INC_H

