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

    float2 center = float2(0.5, 0.5);
    float distanceFromCenter = length(i_fragmentUV - center);

    float time = g_elapsedSecondCount_systemTime;
    float hueShift = fmod(distanceFromCenter * 10.0 + time * 2.0, 1.0);
    float3 rainbowColor = float3(
        0,
        abs(sin(hueShift * 6.28318 + 2.09439)),
        abs(sin(hueShift * 6.28318 + 4.18879))
    );

    o_color = float4(baseColor.rgb * rainbowColor, baseColor.a);
	
}