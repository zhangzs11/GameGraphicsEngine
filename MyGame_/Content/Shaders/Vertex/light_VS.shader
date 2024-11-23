/*
	This is the lgiht vertex shader

	A vertex shader is responsible for two things:
		* Telling the GPU where the vertex (one of the three in a triangle) should be drawn on screen in a given window
			* The GPU will use this to decide which fragments (i.e. pixels) need to be shaded for a given triangle
		* Providing any data that a corresponding fragment shader will need
			* This data will be interpolated across the triangle and thus vary for each fragment of a triangle that gets shaded
*/

#include <Shaders/ConstantBuffers.inc>

// Entry Point
//============

void main(

	// Input
	//======

	// The "semantics" (the keywords in all caps after the colon) are arbitrary,
	// but must match the C call to CreateInputLayout()

	// These values come from one of the VertexFormats::sVertex_mesh that the vertex buffer was filled with in C code
	in const float3 i_vertexPosition_local : POSITION,
	in const float4 i_vertexColor : COLOR,
	in const float2 i_vertexUV : TEXCOORD0,
	in const float3 i_vertexNormal : NORMAL,
	in const float3 i_vertexTangent : TANGENT,
	// Output
	//=======

	// An SV_POSITION value must always be output from every vertex shader
	// so that the GPU can figure out which fragments need to be shaded
	out float4 o_vertexPosition_projected : SV_POSITION,
	out float3 o_vertexPosition_world : POSITION,
	out float4 o_vertexColor : COLOR,
	out float2 o_vertexUV : TEXCOORD0,
	out float3 o_vertexNormal_world : NORMAL,
	out float4 o_vertexTangent_world : TANGENT,
	out float4 o_vertexShadowPosH : TEXCOORD1
)

// Main Body
//============
{
	// Transform the local vertex into world space
	float4 vertexPosition_world;
	{
		// This will be done in a future assignment.
		// For now, however, local space is treated as if it is the same as world space.
		float4 vertexPosition_local = float4( i_vertexPosition_local, 1.0 );
		// Transform the vertex from local to world space

		vertexPosition_world = mul( g_transform_localToWorld, vertexPosition_local );
	}
	// Calculate the position of this vertex projected onto the display
	{
		// Transform the vertex from world space into camera space

		float4 vertexPosition_camera = mul( g_transform_worldToCamera, vertexPosition_world );

		// Project the vertex from camera space into projected space

		o_vertexPosition_projected = mul( g_transform_cameraToProjected, vertexPosition_camera );

		// Assign the color
		o_vertexPosition_world = vertexPosition_world.xyz;
		o_vertexColor = i_vertexColor;

		o_vertexNormal_world = mul( (float3x3) g_transform_localToWorld_Inv_Transpose, i_vertexNormal );

		o_vertexTangent_world = float4(mul((float3x3) g_transform_localToWorld, i_vertexTangent), 1.0);

		o_vertexShadowPosH = mul(g_ShadowTransform, vertexPosition_world);

		o_vertexUV = i_vertexUV;  
	}
}
