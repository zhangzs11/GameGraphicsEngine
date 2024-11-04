/*
	This vertex shader is used to create a Direct3D vertex input layout object
*/

#include <Shaders/light.inc>

// Entry Point
//============

void main(

	// Input
	//======

	// The "semantics" (the keywords in all caps after the colon) are arbitrary,
	// but must match the C call to CreateInputLayout()

	// These values come from one of the VertexFormats::sVertex_mesh that the vertex buffer was filled with in C code
	in const float3 i_position : POSITION,
	in const float4 i_color : COLOR,
	in const float2 i_uv : TEXCOORD0,
	in const float3 i_normal : NORMAL,
	in const float3 i_tangent : TANGENT,
	// Output
	//=======

	// An SV_POSITION value must always be output from every vertex shader
	// so that the GPU can figure out which fragments need to be shaded
	out float4 o_position : SV_POSITION,
	out float4 o_color : COLOR

)
{
	// The shader program is only used to generate a vertex input layout object;
	// the actual shading code is never used
	o_position = float4( i_position, 1.0 );
	o_color = i_color;
}
