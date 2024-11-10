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
	uint2 coords = i_fragmentPosition.xy;

	// float4 lit = g_LitTexture[coords];
    // float depth = g_DepthTexture[coords];
    
	float4 lit = g_LitTexture.Sample(g_sampler, i_fragmentUV);
	float depth = g_DepthTexture.Sample(g_sampler, i_fragmentUV);

	float time = abs(sin( g_elapsedSecondCount_simulationTime));
	
	// if(time >= pow(depth, 1000)) {
	// 	o_color = float4(0.0f,  1.0f, 0.0f, 1.0f);		
	// }
	// else {
	// 	o_color = lit;	
	// }

	if(abs(time - depth) <= 0.01f) {
		o_color = lit * float4(0.3f,  1.0f, 0.3f, 1.0f);		
	} 
	else{
		o_color = lit;
	}
}