/*
	This vertex shader is used to create a Direct3D vertex input layout object
*/

#include <Shaders/shaders.inc>

#if defined( EAE6320_PLATFORM_D3D )

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
	in const float4 i_normal : NORMAL,
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

#elif defined( EAE6320_PLATFORM_GL )

// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()

// These values come from one of the VertexFormats::sVertex_mesh that the vertex buffer was filled with in C code
layout( location = 0 ) in vec3 i_position;
layout( location = 1 ) in vec4 i_color;
layout( location = 2 ) in vec2 i_uv;
layout( location = 3 ) in vec3 i_normal; 
// Output
//=======
layout( location = 1 ) out vec4 o_color;
// The vertex shader must always output a position value,
// but unlike HLSL where the value is explicit
// GLSL has an automatically-required variable named "gl_Position"

// Entry Point
//============

void main()
{
	// The shader program is only used by Direct3D
	gl_Position = vec4( i_position, 1.0 );
	o_color = i_color;
}

#endif
