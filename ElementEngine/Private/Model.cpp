#include <Model.h>
#include <SDL3/SDL_gpu.h>
#include <cstring>

Model::Model()
{
    mVertexBuffer = nullptr;
    mIndexBuffer = nullptr;
}

Model::Model(const Model &) {}
Model::~Model() {}

bool Model::Initialize(SDL_GPUDevice *device)
{
    return InitializeBuffers(device);
}
void Model::Shutdown(SDL_GPUDevice *device)
{
    ShutdownBuffers(device);
}
void Model::Render(SDL_GPURenderPass *renderPass)
{
    RenderBuffers(renderPass);
}

bool Model::InitializeBuffers(SDL_GPUDevice *device)
{
    Vertex *vertices;
    Uint16 *indices;

    //--temp
    mNumVertices = 3;

    mNumIndices = 3;

    // Create the vertex array.
    vertices = new Vertex[mNumVertices];
    if (!vertices)
    {
        return false;
    }

    // Create the index array.
    indices = new Uint16[mNumIndices];
    if (!indices)
    {
        return false;
    }

    // Load the vertex array with data
    vertices[0].position = glm::vec3(-1.0f, -1.0f, 0.0f); // Bottom left.
    vertices[0].color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    vertices[1].position = glm::vec3(0.0f, 1.0f, 0.0f); // Top middle.
    vertices[1].color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[2].position = glm::vec3(1.0f, -1.0f, 0.0f); // Bottom right.
    vertices[2].color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    // Load the index array with data.
    indices[0] = 0; // Bottom left.
    indices[1] = 1; // Top middle.
    indices[2] = 2; // Bottom right.
    //--

    // Create vertex buffer
    SDL_GPUBufferCreateInfo vertexBufferInfo;
    vertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertexBufferInfo.size = mNumVertices * sizeof(Vertex);
    mVertexBuffer = SDL_CreateGPUBuffer(device, &vertexBufferInfo);

    // Create index buffer
    SDL_GPUBufferCreateInfo indexBufferInfo;
    indexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    indexBufferInfo.size = mNumIndices * sizeof(Uint16);
    mIndexBuffer = SDL_CreateGPUBuffer(device, &indexBufferInfo);

    // Create transfer buffer
    SDL_GPUTransferBufferCreateInfo transferBufferInfo;
    transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBufferInfo.size = (mNumVertices * sizeof(Vertex)) +
                              (mNumIndices * sizeof(Uint16));
    SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferInfo);

    // Map transfer buffer
    void *dataPtr = SDL_MapGPUTransferBuffer(device, transferBuffer, false);

    // Copy vertex and index data
    std::memcpy(dataPtr, vertices, mNumVertices * sizeof(Vertex));
    std::memcpy(static_cast<uint8_t *>(dataPtr) + mNumVertices * sizeof(Vertex),
                indices, mNumIndices * sizeof(Uint16));

    SDL_UnmapGPUTransferBuffer(device, transferBuffer);

    // Upload data to GPU buffers
    SDL_GPUCommandBuffer *uploadCmdBuf = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

    // Upload vertex data
    SDL_GPUTransferBufferLocation vertexTransferLocation;
    vertexTransferLocation.transfer_buffer = transferBuffer;
    vertexTransferLocation.offset = 0;

    SDL_GPUBufferRegion vertexBufferRegion;
    vertexBufferRegion.buffer = mVertexBuffer;
    vertexBufferRegion.offset = 0;
    vertexBufferRegion.size = mNumVertices * sizeof(Vertex);

    SDL_UploadToGPUBuffer(copyPass, &vertexTransferLocation, &vertexBufferRegion, false);

    // Upload index data
    SDL_GPUTransferBufferLocation indexTransferLocation;
    indexTransferLocation.transfer_buffer = transferBuffer;
    indexTransferLocation.offset = mNumVertices * sizeof(Vertex);

    SDL_GPUBufferRegion indexBufferRegion;
    indexBufferRegion.buffer = mIndexBuffer;
    indexBufferRegion.offset = 0;
    indexBufferRegion.size = mNumIndices * sizeof(Uint16);

    SDL_UploadToGPUBuffer(copyPass, &indexTransferLocation, &indexBufferRegion, false);

    // End and submit copy pass
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmdBuf);

    // Release the transfer buffer after upload
    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

    delete[] vertices;
    vertices = 0;

    delete[] indices;
    indices = 0;

    return true;
}

void Model::ShutdownBuffers(SDL_GPUDevice *device)
{
    if (mIndexBuffer)
    {
        SDL_ReleaseGPUBuffer(device, mIndexBuffer);
        mIndexBuffer = nullptr;
    }
    if (mVertexBuffer)
    {
        SDL_ReleaseGPUBuffer(device, mVertexBuffer);
        mVertexBuffer = nullptr;
    }
}

void Model::RenderBuffers(struct SDL_GPURenderPass *renderPass)
{
    SDL_GPUBufferBinding vertexBinding = {mVertexBuffer, 0};
    SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);
    SDL_GPUBufferBinding indexBinding = {mIndexBuffer, 0};
    SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);

    SDL_DrawGPUIndexedPrimitives(renderPass, mNumIndices, 1, 0, 0, 0);
}