struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular; // w = SpecPower
    float4 reflect;
};

// Constant Buffers
//=================
cbuffer g_constantBuffer_frame : register( b0 )
{
	// Main Rendering
	// --------------------------------------
	float4x4 g_transform_worldToCamera;
	float4x4 g_transform_cameraToProjected;

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	float2 g_padding1;

	float4 g_CameraNearFar;
};
cbuffer g_constantBuffer_drawCall : register( b2 )
{
    float4x4 g_transform_localToWorld;
	float4x4 g_transform_localToWorld_Inv_Transpose;
	
    Material g_Material;
};

void main(

	// Input
	//======
	in const float3 i_vertexPosition_local : POSITION,
	in const float4 i_vertexColor : COLOR,
	in const float2 i_vertexUV : TEXCOORD0,
	in const float3 i_vertexNormal : NORMAL,
	in const float3 i_vertexTangent : TANGENT,


	// Output
	//=======
	out float4 o_vertexPosition_projected : SV_POSITION,
	// out float3 o_vertexPosition_world : POSITION,

	//
	out float3 o_vertexPosition_view : POSITION,
	//

	// out float4 o_vertexColor : COLOR,
	out float2 o_vertexUV : TEXCOORD0,
	// out float3 o_vertexNormal_world : NORMAL,

	//
	out float3 o_vertexNormal_view : NORMAL
	//

	// out float4 o_vertexTangent_world : TANGENT,
	// out float4 o_vertexShadowPosV : TEXCOORD1,
	// out float  o_vertexDepthV : TEXCOORD2
)

// Main Body
//============
{
	// Transform the local vertex into world space
	float4 vertexPosition_world;
	{
		// This will be done in a future assignment.
		// For now, however, local space is treated as if it is the same as world space.
		float4 vertexPosition_local = float4( i_vertexPosition_local, 1.0 );
		// Transform the vertex from local to world space

		vertexPosition_world = mul( g_transform_localToWorld, vertexPosition_local );
	}
	// Calculate the position of this vertex projected onto the display
	{

		float4 vertexPosition_camera = mul( g_transform_worldToCamera, vertexPosition_world );

		o_vertexPosition_view = vertexPosition_camera.xyz;

		o_vertexPosition_projected = mul( g_transform_cameraToProjected, vertexPosition_camera );

		// Assign the color
		// o_vertexPosition_world = vertexPosition_world.xyz;
		// o_vertexColor = i_vertexColor;

		float3 vertexNormal_world = mul( (float3x3) g_transform_localToWorld_Inv_Transpose, i_vertexNormal );

		o_vertexNormal_view = mul( (float3x3) g_transform_worldToCamera, vertexNormal_world ); // if there is scale , should use inverse maybe

		// o_vertexTangent_world = float4(mul((float3x3) g_transform_localToWorld, i_vertexTangent), 1.0);

		// o_vertexShadowPosV = mul(g_ShadowView, float4(o_vertexPosition_world, 1.0f));

		// o_vertexDepthV = vertexPosition_camera.z;

		o_vertexUV = i_vertexUV;  
	}
}
