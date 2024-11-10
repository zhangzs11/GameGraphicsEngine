/*
	This is the full screen vertex shader
*/
// Entry Point
//============

void main(

	// Input
	//======
	uint vertexID : SV_VertexID,

	// Output
	//=======
	out float4 o_vertexPosition_projected : SV_POSITION,
	out float2 o_vertexUV : TEXCOORD
)

// Main Body
//============
{
	float2 grid = float2((vertexID << 1) & 2, vertexID & 2);
	// In  :  0, 1, 2
	// Out : (0, 0), (2, 0), (0, 2)
	float2 xy = grid * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	// (-1, 1)________ (3, 1)
    //        |   |  /
    // (-1,-1)|___|/ (1, -1)   
    //        |  /
    // (-1,-3)|/    
	o_vertexPosition_projected = float4(xy, 1.0f, 1.0f);


	// (0, 0) ________ (2, 0)
    //        |   |  /
    // (0,1)  |___|/ (1, 1)   
    //        |  /
    // (0,2)  |/   
	o_vertexUV = grid * float2(1.0f, 1.0f);
}