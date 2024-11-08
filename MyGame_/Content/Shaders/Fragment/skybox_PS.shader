/*
	This is the skybox light fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/light.inc>

// Texture and Sampler
//====================
TextureCube g_texture_cube : register(t0); // Texture bound to slot t0
Texture2D<float> g_DepthTexture : register(t1);
Texture2D<float4> g_LitTexture : register(t2);

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
	uint2 coords = i_fragmentPosition.xy;

	float3 lit = float3(0.0f, 0.0f, 0.0f);
    float depth = g_DepthTexture[coords];
    if (depth >= 1.0f)
    {
        lit += g_texture_cube.Sample(g_sampler, i_fragmentPosition_local).rgb;
		//lit += float3(0.0f, 1.0f, 0.0f);
    }
    else
    {
        lit += g_LitTexture[coords].rgb;
		//lit += float3(1.0f, 0.0f, 0.0f);
    }
	o_color = float4(lit, 1.0f);
	// o_color = g_texture_cube.Sample(g_sampler, i_fragmentPosition_local);
}