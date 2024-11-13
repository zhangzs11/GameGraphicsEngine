/*
	This is the one of post process fragment shader
*/

#include <Shaders/light.inc>

// Texture and Sampler
//====================
Texture2D<float4> g_LitTexture : register(t0);
Texture2D<float> g_DepthTexture : register(t1);

SamplerState g_sampler : register(s0);     // Sampler bound to slot s0

// Entry Point
//============

void main(

	// Input
	//======
	in const float4 i_fragmentPosition : SV_POSITION,
	in const float2 i_fragmentUV : TEXCOORD,

	// Output
	//=======
	out float4 o_color : SV_TARGET

)

{
	//
    float4 baseColor = g_LitTexture.Sample(g_sampler, i_fragmentUV);
    float depth = g_DepthTexture.Sample(g_sampler, i_fragmentUV);
	//

	float distortion = sin(i_fragmentUV.y * 20.0 + g_elapsedSecondCount_systemTime * 5.0) * 0.015;
	float2 distortedUV = i_fragmentUV + float2(distortion, 0);

	o_color = g_LitTexture.Sample(g_sampler, distortedUV);
	
}