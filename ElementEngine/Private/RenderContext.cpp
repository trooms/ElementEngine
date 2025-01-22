#include <RenderContext.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>
#include <ext/scalar_constants.hpp>
#include <ext/matrix_clip_space.hpp>
#include <utility>
#include <Shader/Custom/ColorShader.h>
#include <Model.h>
#include <Camera.h>

RenderContext::RenderContext()
{
}

RenderContext::RenderContext(const RenderContext &)
{
}

RenderContext::~RenderContext()
{
}

bool RenderContext::Initialize(int screenWidth, int screenHeight, SDL_Window *window, ColorShader *shader)
{
    if (mDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_MSL, true, nullptr); mDevice == nullptr)
    {
        SDL_Log("GPU could not be selected! SDL error: %s\n", SDL_GetError());
        return false;
    }

    if (!SDL_ClaimWindowForGPUDevice(mDevice, window))
    {
        SDL_Log("Failed to claim application window for GPU device! SDL_GPU error: %s\n", SDL_GetError());
        return false;
    }
    mWindow = window;

    shader->Initialize(mDevice);

    // Depth stencil description
    SDL_GPUDepthStencilState depthStencilState = {};

    depthStencilState.enable_depth_test = true;
    depthStencilState.enable_depth_write = true;
    depthStencilState.compare_op = SDL_GPU_COMPAREOP_LESS;

    depthStencilState.enable_stencil_test = false;
    depthStencilState.compare_mask = 0xFF;
    depthStencilState.write_mask = 0xFF;

    // Front-facing
    depthStencilState.front_stencil_state.fail_op = SDL_GPU_STENCILOP_KEEP;
    depthStencilState.front_stencil_state.depth_fail_op = SDL_GPU_STENCILOP_INCREMENT_AND_WRAP;
    depthStencilState.front_stencil_state.pass_op = SDL_GPU_STENCILOP_KEEP;
    depthStencilState.front_stencil_state.compare_op = SDL_GPU_COMPAREOP_ALWAYS;

    // Back-facing
    depthStencilState.back_stencil_state.fail_op = SDL_GPU_STENCILOP_KEEP;
    depthStencilState.back_stencil_state.depth_fail_op = SDL_GPU_STENCILOP_DECREMENT_AND_WRAP;
    depthStencilState.back_stencil_state.pass_op = SDL_GPU_STENCILOP_KEEP;
    depthStencilState.back_stencil_state.compare_op = SDL_GPU_COMPAREOP_ALWAYS;

    // Rasterization state
    SDL_GPURasterizerState rasterState = {};
    rasterState.cull_mode = SDL_GPU_CULLMODE_BACK;
    rasterState.enable_depth_clip = true;
    rasterState.fill_mode = SDL_GPU_FILLMODE_FILL;
    rasterState.front_face = SDL_GPU_FRONTFACE_CLOCKWISE;

    // Projection matrices
    float fieldOfView = glm::pi<float>() / 4.0f;
    float screenAspect = (float)screenWidth / (float)screenHeight;

    mProjectionMatrix = glm::perspectiveLH_ZO(fieldOfView, screenAspect, SCREEN_NEAR, SCREEN_DEPTH);
    mOrthoMatrix = glm::orthoLH_ZO(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, SCREEN_NEAR, SCREEN_DEPTH);
    mWorldMatrix = glm::mat4x4(1.0f);

    // Render target
    SDL_GPUColorTargetDescription colorTargetDesc = {
        SDL_GetGPUSwapchainTextureFormat(mDevice, mWindow)};

    SDL_GPUGraphicsPipelineTargetInfo pipelineTargetInfo = {0};
    pipelineTargetInfo.color_target_descriptions = &colorTargetDesc;
    pipelineTargetInfo.num_color_targets = 1;
    pipelineTargetInfo.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT;
    pipelineTargetInfo.has_depth_stencil_target = false;

    // Pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.depth_stencil_state = depthStencilState;
    pipelineInfo.rasterizer_state = rasterState;
    pipelineInfo.target_info = pipelineTargetInfo;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineInfo.vertex_input_state = shader->GetVertexInputState();
    pipelineInfo.vertex_shader = shader->GetVertexShader();
    pipelineInfo.fragment_shader = shader->GetFragmentShader();
    // pipelineInfo.multisample_state

    mPipeline = SDL_CreateGPUGraphicsPipeline(mDevice, &pipelineInfo);

    // Buffer creation - depth
    SDL_GPUTextureCreateInfo depthBufferTextureInfo = {};

    int drawableWidth, drawableHeight;
    SDL_GetWindowSizeInPixels(mWindow, &drawableWidth, &drawableHeight);

    depthBufferTextureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    depthBufferTextureInfo.width = drawableWidth;
    depthBufferTextureInfo.height = drawableHeight;
    depthBufferTextureInfo.num_levels = 1;
    depthBufferTextureInfo.layer_count_or_depth = 1;
    depthBufferTextureInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    depthBufferTextureInfo.format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT;
    depthBufferTextureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;

    mDepthBuffer = SDL_CreateGPUTexture(mDevice, &depthBufferTextureInfo);
    if (!mDepthBuffer)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create depth-stencil texture: %s", SDL_GetError());
        return false;
    }

    return true;
}

void RenderContext::Shutdown()
{
    if (mPipeline)
    {
        SDL_ReleaseGPUGraphicsPipeline(mDevice, mPipeline);
    }
    if (mDepthBuffer)
    {
        SDL_ReleaseGPUTexture(mDevice, mDepthBuffer);
    }
}

void RenderContext::InitScene(SceneResources &resources)
{
    resources.cmdbuf = SDL_AcquireGPUCommandBuffer(mDevice);
    if (!resources.cmdbuf)
    {
        SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
    }

    if (!SDL_WaitAndAcquireGPUSwapchainTexture(resources.cmdbuf, mWindow, &resources.swapchainTexture, NULL, NULL))
    {
        SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
    }

    if (!resources.swapchainTexture)
    {
        return;
    }
}

void RenderContext::BeginScene(SceneResources &resources)
{
    SDL_GPUColorTargetInfo colorTargetInfo = {
        .texture = resources.swapchainTexture};
    SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo = {};
    GetBackBufferRenderTargetInfo(colorTargetInfo, depthStencilTargetInfo);

    resources.renderPass = SDL_BeginGPURenderPass(resources.cmdbuf, &colorTargetInfo, 1, NULL); //&depthStencilTargetInfo);
    SDL_BindGPUGraphicsPipeline(resources.renderPass, mPipeline);
}

void RenderContext::EndScene(SceneResources &resources)
{
    SDL_EndGPURenderPass(resources.renderPass);
    SDL_SubmitGPUCommandBuffer(resources.cmdbuf);
}

SDL_GPUDevice *RenderContext::GetDevice()
{
    return mDevice;
}

void RenderContext::GetProjectionMatrix(glm::mat4x4 &projectionMatrix)
{
    projectionMatrix = mProjectionMatrix;
}

void RenderContext::GetWorldMatrix(glm::mat4x4 &worldMatrix)
{
    worldMatrix = mWorldMatrix;
}

void RenderContext::GetOrthoMatrix(glm::mat4x4 &orthoMatrix)
{
    orthoMatrix = mOrthoMatrix;
}

void RenderContext::GetBackBufferRenderTargetInfo(SDL_GPUColorTargetInfo &colorTargetInfo, SDL_GPUDepthStencilTargetInfo &stencilTargetInfo)
{
    colorTargetInfo.clear_color = (SDL_FColor){0.0f, 0.0f, 0.0f, 0.0f};
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

    stencilTargetInfo = {0};
    /*stencilTargetInfo.texture = mDepthBuffer;
    stencilTargetInfo.cycle = true;
    stencilTargetInfo.clear_depth = 1;
    stencilTargetInfo.clear_stencil = 0;
    stencilTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    stencilTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    stencilTargetInfo.stencil_load_op = SDL_GPU_LOADOP_CLEAR;
    stencilTargetInfo.stencil_store_op = SDL_GPU_STOREOP_STORE;*/
}