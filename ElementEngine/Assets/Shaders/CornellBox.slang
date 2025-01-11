struct AssembledVertex
{
    float3 position : POSITION;
    uint4 color : COLOR;
};

struct CoarseVertex
{
    float4 color : COLOR;
};

struct Fragment
{
    float4 color : SV_Target;
};

// Vertex Shader
struct VertexStageOutput
{
    CoarseVertex coarseVertex;
    float4 sv_position : SV_Position;
};

[shader("vertex")]
VertexStageOutput vertexMain(
    AssembledVertex assembledVertex)
{
    VertexStageOutput output;

    output.coarseVertex.color = assembledVertex.color;
    output.sv_position = float4(assembledVertex.position, 1.0);

    return output;
}

// Fragment Shader
[shader("fragment")]
Fragment fragmentMain(
    CoarseVertex coarseVertex)
{
    Fragment output;

    output.color = coarseVertex.color;

    return output;
}
