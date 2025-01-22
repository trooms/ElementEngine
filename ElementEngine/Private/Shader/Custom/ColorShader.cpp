#include <Shader/Custom/ColorShader.h>
#include <Shader/Format/SlangShaderSystem.h>

void ColorShader::Initialize(SDL_GPUDevice *device)
{
    mShaderSystem = new SlangShaderSystem;
    mShaderSystem->Initialize();
    mVertexShader = mShaderSystem->LoadShader(device, "Color.slang", "vertexMain", SDL_GPU_SHADERSTAGE_VERTEX);
    mFragmentShader = mShaderSystem->LoadShader(device, "Color.slang", "fragmentMain", SDL_GPU_SHADERSTAGE_FRAGMENT);

    mVertexAttributes[0] = {
        .location = 0,
        .buffer_slot = 0,
        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
        .offset = offsetof(VertexInput, position)};
    mVertexAttributes[1] = {
        .location = 1,
        .buffer_slot = 0,
        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
        .offset = offsetof(VertexInput, color)};

    mVertexBufferDescriptions[0] = {
        .slot = 0,
        .pitch = sizeof(VertexInput),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0};

    mVertexInputState = {
        .vertex_buffer_descriptions = mVertexBufferDescriptions,
        .num_vertex_buffers = 1,
        .vertex_attributes = mVertexAttributes,
        .num_vertex_attributes = 2};
}

void ColorShader::SetShaderUniforms(SDL_GPUCommandBuffer *cmdbuffer, glm::mat4x4 worldMatrix, glm::mat4x4 viewMatrix, glm::mat4x4 projectionMatrix)
{
    MatrixBuffer matrixBuffer = {
        .projection = glm::transpose(worldMatrix),
        .view = glm::transpose(viewMatrix),
        .world = glm::transpose(worldMatrix)};

    SDL_PushGPUVertexUniformData(cmdbuffer, 0, &matrixBuffer, sizeof(matrixBuffer));
}

void ColorShader::ReleaseShaders(SDL_GPUDevice *device)
{
    SDL_ReleaseGPUShader(device, mVertexShader);
    SDL_ReleaseGPUShader(device, mFragmentShader);
}

void ColorShader::Shutdown()
{
    delete mShaderSystem;
}