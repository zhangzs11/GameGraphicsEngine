/*
	This is the standard fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/shaders.inc>
#include <Shaders/Common/CommonBuffers.shader>

#if defined( EAE6320_PLATFORM_D3D )


// Entry Point
//============

void main(

	// Input
	//======

	in const float4 i_fragmentPosition : SV_POSITION,

	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : SV_TARGET

)
{
	float time = g_elapsedSecondCount_simulationTime * 0.5;

	float2 screenCenter = float2(400.0f, 300.0f);
    float distanceFromCenter = length(i_fragmentPosition.xy - screenCenter) / 300.0f;
    float gradient = 0.5f + 0.5f * sin(time + distanceFromCenter * 10.0f);

	float r = gradient;
    float g = abs(sin(time + distanceFromCenter * 5.0f));
    float b = abs(cos(time + distanceFromCenter * 7.0f));

    o_color = float4(r, g, b, 1.0);
}

#elif defined( EAE6320_PLATFORM_GL )


// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
out vec4 o_color;

// Entry Point
//============

void main()
{
    float time = g_elapsedSecondCount_simulationTime * 0.5;
    
    float distanceFromCenter = length(gl_FragCoord.xy - vec2(400.0, 300.0)) / 300.0;
    float gradient = 0.5 + 0.5 * sin(time + distanceFromCenter * 10.0);
    
    float r = gradient;
    float g = abs(sin(time + distanceFromCenter * 5.0));
    float b = abs(cos(time + distanceFromCenter * 7.0));

    o_color = vec4(r, g, b, 1.0);
}

#endif
