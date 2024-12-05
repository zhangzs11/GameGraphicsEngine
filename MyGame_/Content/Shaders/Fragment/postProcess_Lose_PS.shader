/*
	This is the one of post process fragment shader
*/

#include <Shaders/ConstantBuffers.inc>

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
    float4 baseColor = g_LitTexture.Sample(g_sampler, i_fragmentUV);
    float depth = g_DepthTexture.Sample(g_sampler, i_fragmentUV);


    float distortion = sin(i_fragmentUV.y * 40.0 + g_elapsedSecondCount_systemTime * 10.0) * 0.02;
    float2 distortedUV = i_fragmentUV + float2(distortion, 0);

    float distanceFromCenter = length(i_fragmentUV - float2(0.5, 0.5));
    float vignette = 1.0 - smoothstep(0.3, 0.8, distanceFromCenter);
    float4 blurredColor = g_LitTexture.Sample(g_sampler, distortedUV) * vignette;

    float flicker = abs(sin(g_elapsedSecondCount_systemTime * 20.0)) * 0.3;
    float4 horrorColor = float4(0.2, 0.0, 0.0, 0.0) * flicker;

    o_color = lerp(blurredColor, horrorColor, 0.5);
    o_color.rgb *= vignette;
	
}