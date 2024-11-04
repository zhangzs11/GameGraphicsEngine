/*
	This file should be #included by all shaders
*/

#ifndef SHADOW_INC_H
#define SHADOW_INC_H

#include <Shaders/lightHelper.inc>

// Constant Buffers
//=================
cbuffer g_constantBuffer_frame : register( b0 )
{
	float4x4 g_transform_worldToCamera;
	float4x4 g_transform_cameraToProjected;
};
cbuffer g_constantBuffer_drawCall : register( b2 )
{
    float4x4 g_transform_localToWorld;
	float4x4 g_transform_localToWorld_Inv_Transpose;
	
    Material g_Material;
};

// Texture and Sampler
//====================
// Texture2D g_texture0 : register(t0);    // Texture bound to slot t0
// Texture2D g_texture1 : register(t1);    // Texture bound to slot t0
// SamplerState g_sampler : register(s0); // Sampler bound to slot s0


#endif // SHADOW_INC_H

