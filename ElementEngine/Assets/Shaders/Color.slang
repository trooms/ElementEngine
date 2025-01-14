struct MatrixBuffer
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

struct VertexInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct FragmentInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

[shader("vertex")]
FragmentInput vertexMain(
    VertexInput input, uniform MatrixBuffer matrixBuffer)
{
    FragmentInput output;

    // Change the position vector to be 4 units for proper matrix calculations.
    float4 paddedPos = float4(input.position, 1.0f);

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(paddedPos, matrixBuffer.worldMatrix);
    output.position = mul(output.position, matrixBuffer.viewMatrix);
    output.position = mul(output.position, matrixBuffer.projectionMatrix);

    // Store the input color for the pixel shader to use.
    output.color = input.color;

    return output;
}

[shader("fragment")]
float4 fragmentMain(
    FragmentInput input)
    : SV_Target
{
    return input.color;
}

