#pragma once

#include <SDL3/SDL_gpu.h>

class Shader
{
public:
    virtual ~Shader() = default;

    virtual void Initialize(SDL_GPUDevice *device) = 0;
    virtual void Shutdown() = 0;

    inline SDL_GPUShader *GetVertexShader() { return mVertexShader; }
    inline SDL_GPUShader *GetFragmentShader() { return mFragmentShader; }
    inline SDL_GPUVertexInputState GetVertexInputState() { return mVertexInputState; }

protected:
    SDL_GPUShader *mVertexShader;
    SDL_GPUShader *mFragmentShader;
    SDL_GPUVertexInputState mVertexInputState;
};
