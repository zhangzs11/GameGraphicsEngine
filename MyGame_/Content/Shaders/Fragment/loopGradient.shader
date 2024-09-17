/*
	This is the loop gradient fragment shader

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

	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : SV_TARGET

)

#elif defined( EAE6320_PLATFORM_GL )

// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
out vec4 o_color;

// Entry Point
//============

void main()

#endif

// Main Body 
//============

{
	float time = g_elapsedSecondCount_simulationTime * 0.5;

    #if defined( EAE6320_PLATFORM_D3D )
        float4_t screen_uv = i_fragmentPosition;
    #elif defined( EAE6320_PLATFORM_GL )
        float4_t screen_uv = gl_FragCoord;
    #endif

    float distanceFromCenter = length(screen_uv.xy - float2_t(400.0f, 300.0f)) / 300.0f;
    float gradient = 0.5f + 0.5f * sin(time + distanceFromCenter * 10.0f);

	float r = gradient;
    float g = abs(sin(time + distanceFromCenter * 15.0f));
    float b = abs(cos(time + distanceFromCenter * 7.0f));

    o_color = float4_t(r, g, b, 1.0);
}
