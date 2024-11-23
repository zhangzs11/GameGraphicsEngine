/*
	This file should be #included by all shaders
*/

#ifndef CONSTANTBUFFERS_INC_H
#define CONSTANTBUFFERS_INC_H

#include <Shaders/lightHelper.inc>

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
	// For float4 alignment
	float2 g_padding1;
	
    DirectionalLight g_DirLight;
    PointLight g_PointLight[10];
    SpotLight g_SpotLight[10];
	
    float4x4 g_ShadowTransform; // ShadowView * ShadowProj * T
	
    float3 g_EyePosW;
    float g_padding2;
	
    
	// Shadow Map Rendering
	// ---------------------------------------
    float4x4 g_transform_worldToShadowMapCamera;
    float4x4 g_transform_cameraToShadowMapProjected;
	
    
	// FXAA
	// ---------------------------------------
    float2 g_TexelSize;
    
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

    float3 g_padding3;
	
};
cbuffer g_constantBuffer_drawCall : register( b2 )
{
    float4x4 g_transform_localToWorld;
	float4x4 g_transform_localToWorld_Inv_Transpose;
	
    Material g_Material;
};


#endif // CONSTANTBUFFERS_INC_H

