// shaders/geometryShader.hlsl
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
};

[maxvertexcount(3)]
void main(triangle VS_OUTPUT input[3], inout TriangleStream<VS_OUTPUT> outputStream)
{
    // Geometry shader implementation here
}
