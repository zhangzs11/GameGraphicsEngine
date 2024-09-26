/*
	This is the animated color fragment shader

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

	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : SV_TARGET

)

#elif defined( EAE6320_PLATFORM_GL )

// Input
layout( location = 1 ) in vec4 i_fragmentColor;

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
	float r = abs(sin( g_elapsedSecondCount_simulationTime));
	float g = abs(sin( g_elapsedSecondCount_simulationTime + 1));
	float b = abs(sin( g_elapsedSecondCount_simulationTime + 2));
	// Output solid white
	o_color = float4_t(
		// RGB (color)
		r, g, b,
		// Alpha (opacity)
		1.0 ) * i_fragmentColor;
}