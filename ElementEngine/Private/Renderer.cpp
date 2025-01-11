#include <Renderer.h>
#include <Client.h>
#include <ShaderSystem.h>

bool Renderer::Initialize()
{
    if (GPU_DEVICE = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_MSL, true, nullptr); GPU_DEVICE == nullptr)
    {
        SDL_Log("GPU could not be selected! SDL error: %s\n", SDL_GetError());
        return false;
    }

    if (!SDL_ClaimWindowForGPUDevice(GPU_DEVICE, TargetApp->GetWindow()))
    {
        SDL_Log("Failed to claim application window for GPU device! SDL_GPU error: %s\n", SDL_GetError());
        return false;
    }

    ShaderSystem ShaderSystem;
    ShaderSystem.Initialize();
    SDL_GPUShader *vertexShader = ShaderSystem.LoadShader(GPU_DEVICE, "CornellBox.slang", "vertexMain", SDL_GPU_SHADERSTAGE_VERTEX);
    SDL_GPUShader *fragmentShader = ShaderSystem.LoadShader(GPU_DEVICE, "CornellBox.slang", "fragmentMain", SDL_GPU_SHADERSTAGE_FRAGMENT);
    if (vertexShader == nullptr || fragmentShader == nullptr)
    {
        SDL_Log("Failed to create shaders!");
        return false;
    }

    SDL_GPUColorTargetDescription colorTargetDesc = {
        SDL_GetGPUSwapchainTextureFormat(GPU_DEVICE, TargetApp->GetWindow())};

    SDL_GPUVertexAttribute vertexAttributes[2] = {
        // Position Attribute (location = 0)
        {
            .location = 0,                                // Matches shader input location 0
            .buffer_slot = 0,                             // Refers to the first buffer slot
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, // 3 floats for position (x, y, z)
            .offset = offsetof(PositionColorVertex, x)    // Offset to 'x' in struct
        },
        // Color Attribute (location = 1)
        {
            .location = 1,                                // Matches shader input location 1
            .buffer_slot = 0,                             // Refers to the first buffer slot
            .format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4, // 4 normalized unsigned bytes (r, g, b, a)
            .offset = offsetof(PositionColorVertex, r)    // Offset to 'r' in struct
        }};

    SDL_GPUVertexBufferDescription vertexBufferDescriptions[1] = {
        {
            .slot = 0,                                    // Bind to slot 0
            .pitch = sizeof(PositionColorVertex),         // Size of each vertex
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX, // Data updates per vertex
            .instance_step_rate = 0                       // Ignored for per-vertex input
        }};

    SDL_GPUVertexInputState vertexInputState = {
        .vertex_buffer_descriptions = vertexBufferDescriptions, // Buffer descriptions
        .num_vertex_buffers = 1,                                // One buffer description
        .vertex_attributes = vertexAttributes,                  // Attribute descriptions
        .num_vertex_attributes = 2                              // Two attributes (position, color)
    };
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = &colorTargetDesc,
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_input_state = vertexInputState,
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader};
    // pipelineCreateInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;

    Pipeline = SDL_CreateGPUGraphicsPipeline(GPU_DEVICE, &pipelineCreateInfo);
    if (Pipeline == NULL)
    {
        SDL_Log("Failed to create pipeline!");
        return -1;
    }

    SDL_ReleaseGPUShader(GPU_DEVICE, vertexShader);
    SDL_ReleaseGPUShader(GPU_DEVICE, fragmentShader);

    // Vertex and index data
    // Triangle
    std::vector<PositionColorVertex> vertices = {
        {-1.0f, -1.0f, 0, 1, 0, 0, 1},
        {1.0f, -1.0f, 0, 0, 1, 0, 1},
        {0.0f, 1.0f, 0, 0, 0, 1, 1}};

    std::vector<Uint16> indices = {0, 1, 2};

    // Create vertex buffer
    SDL_GPUBufferCreateInfo vertexBufferInfo;
    vertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertexBufferInfo.size = vertices.size() * sizeof(PositionColorVertex);
    VertexBuffer = SDL_CreateGPUBuffer(GPU_DEVICE, &vertexBufferInfo);

    // Create index buffer
    SDL_GPUBufferCreateInfo indexBufferInfo;
    indexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    indexBufferInfo.size = indices.size() * sizeof(Uint16);
    IndexBuffer = SDL_CreateGPUBuffer(GPU_DEVICE, &indexBufferInfo);

    // Create transfer buffer
    SDL_GPUTransferBufferCreateInfo transferBufferInfo;
    transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBufferInfo.size = (vertices.size() * sizeof(PositionColorVertex)) +
                              (indices.size() * sizeof(Uint16));
    SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(GPU_DEVICE, &transferBufferInfo);

    // Map transfer buffer
    void *dataPtr = SDL_MapGPUTransferBuffer(GPU_DEVICE, transferBuffer, false);

    // Copy vertex and index data
    std::memcpy(dataPtr, vertices.data(), vertices.size() * sizeof(PositionColorVertex));
    std::memcpy(static_cast<uint8_t *>(dataPtr) + vertices.size() * sizeof(PositionColorVertex),
                indices.data(), indices.size() * sizeof(Uint16));

    SDL_UnmapGPUTransferBuffer(GPU_DEVICE, transferBuffer);

    // Upload data to GPU buffers
    SDL_GPUCommandBuffer *uploadCmdBuf = SDL_AcquireGPUCommandBuffer(GPU_DEVICE);
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

    // Upload vertex data
    SDL_GPUTransferBufferLocation vertexTransferLocation;
    vertexTransferLocation.transfer_buffer = transferBuffer;
    vertexTransferLocation.offset = 0;

    SDL_GPUBufferRegion vertexBufferRegion;
    vertexBufferRegion.buffer = VertexBuffer;
    vertexBufferRegion.offset = 0;
    vertexBufferRegion.size = vertices.size() * sizeof(PositionColorVertex);

    SDL_UploadToGPUBuffer(copyPass, &vertexTransferLocation, &vertexBufferRegion, false);

    // Upload index data
    SDL_GPUTransferBufferLocation indexTransferLocation;
    indexTransferLocation.transfer_buffer = transferBuffer;
    indexTransferLocation.offset = vertices.size() * sizeof(PositionColorVertex);

    SDL_GPUBufferRegion indexBufferRegion;
    indexBufferRegion.buffer = IndexBuffer;
    indexBufferRegion.offset = 0;
    indexBufferRegion.size = indices.size() * sizeof(Uint16);

    SDL_UploadToGPUBuffer(copyPass, &indexTransferLocation, &indexBufferRegion, false);

    // End and submit copy pass
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmdBuf);

    // Release the transfer buffer after upload
    SDL_ReleaseGPUTransferBuffer(GPU_DEVICE, transferBuffer);

    return true;
}

bool Renderer::Draw()
{
    // Acquire command buffer
    SDL_GPUCommandBuffer *cmdbuf = SDL_AcquireGPUCommandBuffer(GPU_DEVICE);
    if (cmdbuf == NULL)
    {
        SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        return false;
    }

    // Acquire swapchain texture
    SDL_GPUTexture *swapchainTexture;
    if (!SDL_AcquireGPUSwapchainTexture(cmdbuf, TargetApp->GetWindow(), &swapchainTexture, NULL, NULL))
    {
        SDL_Log("AcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        return false;
    }

    // Begin render pass
    SDL_GPUColorTargetInfo colorTargetInfo = {0};
    colorTargetInfo.texture = swapchainTexture;
    colorTargetInfo.clear_color = (SDL_FColor){0.0f, 0.0f, 0.0f, 1.0f};
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, NULL);
    SDL_BindGPUGraphicsPipeline(renderPass, Pipeline);

    // Bind vertex and index buffers
    SDL_GPUBufferBinding vertexBinding = {VertexBuffer, 0};
    SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);
    SDL_GPUBufferBinding indexBinding = {IndexBuffer, 0};
    SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);

    // Draw primitives
    SDL_DrawGPUIndexedPrimitives(renderPass, 3, 1, 0, 0, 0);

    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(cmdbuf);

    return true;
}
