/*
	This is the skybox vertex shader
*/


#include <Shaders/ConstantBuffers.inc>

// Entry Point
//============

void main(

	// Input
	//======
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
	out float3 o_vertexPosition_local : POSITION
)

// Main Body
//============
{
	// float4 vertexPosition_projected = mul(g_transform_localToWorld, float4(i_vertexPosition_local, 1.0f));
	float4 vertexPosition_projected = float4(i_vertexPosition_local, 1.0f);

	float3 cameraRotatedPosition = mul((float3x3)g_transform_worldToCamera, vertexPosition_projected.xyz);
    vertexPosition_projected = float4(cameraRotatedPosition, 1.0f);

	vertexPosition_projected = mul(g_transform_cameraToProjected, vertexPosition_projected).xyww;

	o_vertexPosition_projected = vertexPosition_projected;
	o_vertexPosition_local = i_vertexPosition_local;
}