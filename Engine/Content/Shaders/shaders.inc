/*
	This file should be #included by all shaders
*/

#ifndef SHADER_INC_H
#define SHADER_INC_H

// Version Information
//====================

#if defined( EAE6320_PLATFORM_GL )

	// GLSL shaders require the version to be #defined before anything else in the shader
	#version 420

#endif

// Constant Buffers
//=================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareConstantBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name

#endif

// Type definitions
//=================

#if defined( EAE6320_PLATFORM_D3D )

	#define float4x4_t float4x4
	#define float4_t float4
    #define float2_t float2

#elif defined( EAE6320_PLATFORM_GL )

	#define float4x4_t mat4
	#define float4_t vec4
    #define float2_t vec2

#endif

// Function definitions
//=====================

#if defined( EAE6320_PLATFORM_D3D )

	#define MUL_MATRIX_VECTOR( matrix, vector ) mul( matrix, vector )

#elif defined( EAE6320_PLATFORM_GL )

	#define MUL_MATRIX_VECTOR( matrix, vector ) ( matrix * vector )

#endif



// Constant Buffers
//=================

DeclareConstantBuffer( g_constantBuffer_frame, 0 )
{
	float4x4_t g_transform_worldToCamera;
	float4x4_t g_transform_cameraToProjected;

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	// For float4 alignment
	float2_t g_padding;
};

DeclareConstantBuffer( g_constantBuffer_drawCall, 2 )
{
    float4x4_t g_transform_localToWorld;
};

#endif // SHADER_INC_H

