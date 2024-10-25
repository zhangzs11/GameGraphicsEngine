/*
	This is the basic light fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/shaders.inc>

#if defined( EAE6320_PLATFORM_D3D )


// Entry Point
//============

void main(

	// Input
	//======

	in const float4 i_fragmentPosition : SV_POSITION,
	in const float4 i_fragmentColor : COLOR,
	in const float2 i_fragmentUV : TEXCOORD0,
	in const float3 i_fragmentNormal : NORMAL,

	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : SV_TARGET

)

#elif defined( EAE6320_PLATFORM_GL )

// Input
layout( location = 1 ) in vec4 i_fragmentColor;
layout( location = 2 ) in vec2 i_fragmentUV;
layout( location = 3 ) in vec3 i_fragmentNormal;
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
out vec4 o_color;

// Entry Point
//============

void main()

#endif

{
	// float r = abs(sin( g_elapsedSecondCount_simulationTime));
	// float g = abs(sin( g_elapsedSecondCount_simulationTime + 1));
	// float b = abs(sin( g_elapsedSecondCount_simulationTime + 2));
	// // Output solid white
	// o_color = float4_t(
	// 	// RGB (color)
	// 	r, g, b,
	// 	// Alpha (opacity)
	// 	1.0 ) * i_fragmentColor;

	// Test normal (convert from [-1, 1] to [0, 1])
    // float3_t normalColor = (i_fragmentNormal + 1.0) / 2.0;
	// o_color = float4_t(normalColor, 1.0);

	// Test UV (directly use UV as color)
    // float4_t uvColor = float4_t(0.0, i_fragmentUV, 1.0);
	// o_color = uvColor;

	// Test Sampler and texture (D3D way)
	float4_t textureColor = g_texture.Sample(g_sampler, i_fragmentUV);
	o_color = textureColor;

}