/*
	This is the skybox light fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/light.inc>

// Texture and Sampler
//====================
TextureCube g_texture_cube : register(t0); // Texture bound to slot t0
SamplerState g_sampler : register(s0);     // Sampler bound to slot s0

// Entry Point
//============

void main(

	// Input
	//======
	in const float4 i_fragmentPosition : SV_POSITION,
	in const float3 i_fragmentPosition_local : POSITION,

	// Output
	//=======
	out float4 o_color : SV_TARGET

)

{
	o_color = g_texture_cube.Sample(g_sampler, i_fragmentPosition_local);;
}