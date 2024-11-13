/*
	This is the basic light fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/light.inc>

// Texture and Sampler
//====================
Texture2D g_texture0 : register(t0);    // Texture bound to slot t0
Texture2D g_texture1 : register(t1);    // Texture bound to slot t1 (normal map)
Texture2D g_ShadowMap : register(t2);   // Shadow map
SamplerState g_sampler : register(s0); // Sampler bound to slot s0

// TODO:
// Add this sampler in CPU side
SamplerComparisonState g_SamShadow : register(s1);

// Entry Point
//============

void main(

	// Input
	//======

	in const float4 i_fragmentPosition : SV_POSITION,
	in const float3 i_fragmentPosition_world : POSITION,
	in const float4 i_fragmentColor : COLOR,
	in const float2 i_fragmentUV : TEXCOORD0,
	in float3 i_fragmentNormal_world : NORMAL,
	in float4 i_fragmentTangent_world : TANGENT,
	in const float4 i_fragmentShadowPosH : TEXCOORD1,

	// TODO:
	// Add float4 ShadowPosH : TEXCOORD1;

	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : SV_TARGET

)

{
	// float4 texColor = g_texture0.Sample(g_sampler, i_fragmentUV);
	float4 texColor = float4(1.0f, 0.0f, 0.0f, 1.0f);

	clip(texColor.a - 0.1f);

	// Normal map
	i_fragmentNormal_world = normalize(i_fragmentNormal_world);
    i_fragmentTangent_world.xyz = normalize(i_fragmentTangent_world.xyz);

	float3 normalMapSample = g_texture1.Sample(g_sampler, i_fragmentUV).rgb;
    float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, i_fragmentNormal_world, i_fragmentTangent_world);


	// Calculate light
	// ---------------

	float3 hitToEyeW = normalize(g_EyePosW - i_fragmentPosition_world);

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Calculate Shadow
	float shadow = CalcShadowFactor(g_SamShadow, g_ShadowMap, i_fragmentShadowPosH, 0.0f); //Bias

	ComputeDirectionalLight(g_Material, g_DirLight, bumpedNormalW, hitToEyeW, A, D, S);
    ambient += A; // Should just A, no shadow
    diffuse += shadow * D;
    spec += shadow * S;

	[unroll]
    for (int i = 0; i < 10; ++i)
    {
		ComputePointLight(g_Material, g_PointLight[i], i_fragmentPosition_world, bumpedNormalW, hitToEyeW, A, D, S);
		ambient += A;
		diffuse += D;
		spec += S;
	}

	[unroll]
    for (int j = 0; j < 10; ++j)
    {
		ComputeSpotLight(g_Material, g_SpotLight[j], i_fragmentPosition_world, bumpedNormalW, hitToEyeW, A, D, S);
		ambient += A;
		diffuse += D;
		spec += S;
	}

	float4 litColor = texColor * (ambient + diffuse) + spec;
    litColor.a = texColor.a * g_Material.diffuse.a;

	o_color = litColor;
	// o_color = float4(bumpedNormalW, 1.0f);
}