/*
	This is the basic light fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/ConstantBuffers.inc>

// Texture and Sampler
//====================
Texture2D g_texture0 : register(t0);    // Texture bound to slot t0
Texture2D g_texture1 : register(t1);    // Texture bound to slot t1
SamplerState g_sampler : register(s0); // Sampler bound to slot s0

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
	in const float4 i_fragmentTangent_world : TANGENT,
	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : SV_TARGET

)

{
	// Calculate light
	// ---------------
	i_fragmentNormal_world = normalize(i_fragmentNormal_world);

	float3 hitToEyeW = normalize(g_EyePosW - i_fragmentPosition_world);

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);

	ComputeDirectionalLight(g_Material, g_DirLight, i_fragmentNormal_world, hitToEyeW, A, D, S);
    ambient += A;
    diffuse += D;
    spec += S;

	ComputePointLight(g_Material, g_PointLight, i_fragmentPosition_world, i_fragmentNormal_world, hitToEyeW, A, D, S);
    ambient += A;
    diffuse += D;
    spec += S;

	ComputeSpotLight(g_Material, g_SpotLight, i_fragmentPosition_world, i_fragmentNormal_world, hitToEyeW, A, D, S);
    ambient += A;
    diffuse += D;
    spec += S;

	float4 texColor = g_texture0.Sample(g_sampler, i_fragmentUV);
	float4 litColor = texColor * (ambient + diffuse) + spec;
    litColor.a = texColor.a * g_Material.diffuse.a;

	o_color = litColor;
}