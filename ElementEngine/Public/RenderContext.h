#pragma once
#include <SDL3/SDL_gpu.h>
#include <mat4x4.hpp>

class RenderContext
{
public:
    static constexpr float SCREEN_DEPTH = 1000.0f;
    static constexpr float SCREEN_NEAR = 0.3f;

    RenderContext();
    RenderContext(const RenderContext &);
    ~RenderContext();

    bool Initialize(int, int, struct SDL_Window *);

    void Shutdown();

    void BeginScene(float, float, float, float);
    void EndScene();

    SDL_GPUDevice *GetDevice();

    void GetProjectionMatrix(glm::mat4x4 &);
    void GetWorldMatrix(glm::mat4x4 &);
    void GetOrthoMatrix(glm::mat4x4 &);

    void GetBackBufferRenderTargetInfo(SDL_GPUColorTargetInfo &, SDL_GPUDepthStencilTargetInfo &);

private:
    SDL_Window *mWindow;
    SDL_GPUGraphicsPipeline *mPipeline;
    SDL_GPUDevice *mDevice;
    SDL_GPUTexture *mColorBuffer;
    SDL_GPUTexture *mDepthBuffer;

    class ColorShader *mColorShader;

    glm::mat4x4 mProjectionMatrix;
    glm::mat4x4 mOrthoMatrix;
    glm::mat4x4 mWorldMatrix;

    class Model *mModel;
    class Camera *mCamera;
};