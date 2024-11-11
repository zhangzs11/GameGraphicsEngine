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
    float4 baseColor = g_LitTexture.Sample(g_sampler, i_fragmentUV);
    float depth = g_DepthTexture.Sample(g_sampler, i_fragmentUV);

    // float time =  fmod(g_elapsedSecondCount_simulationTime, 10.0);
    // float noise = frac(sin(dot(i_fragmentUV * 10.0 + time * 5.0, float2(12.9898,78.233))) * 43758.5453) * 1.5;

    // float3 crazyColor = float3(0.1, 0.9, 0.1) * sin(time * 3.0) + float3(0.2, 1.0, 0.2) * noise;
    // float3 flickerColor = baseColor.rgb * crazyColor;

    // float2 distortion = i_fragmentUV + sin(i_fragmentUV * 25.0 + time * 5.0) * 0.05;
    // float4 distortedColor = g_LitTexture.Sample(g_sampler, distortion);

    // float4 neighborColor1 = g_LitTexture.Sample(g_sampler, i_fragmentUV + float2(0.01, 0.0));
    // float4 neighborColor2 = g_LitTexture.Sample(g_sampler, i_fragmentUV + float2(-0.01, 0.0));
    // float4 neighborColor3 = g_LitTexture.Sample(g_sampler, i_fragmentUV + float2(0.0, 0.01));
    // float4 neighborColor4 = g_LitTexture.Sample(g_sampler, i_fragmentUV + float2(0.0, -0.01));
    
    // float4 edgeColor = abs(neighborColor1 - neighborColor2) + abs(neighborColor3 - neighborColor4);
    // edgeColor *= 5.0; 

    // float3 finalColor = lerp(baseColor.rgb, distortedColor.rgb, 0.7) * (1.0 + flickerColor * 0.5);
    // // finalColor += edgeColor.rgb;
    // finalColor = abs(sin(finalColor * 2.0 * time));

    // o_color = float4(finalColor, 1.0);
    o_color = baseColor;
}