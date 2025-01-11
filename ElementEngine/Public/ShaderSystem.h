#pragma once

#include <SDL3/SDL_gpu.h>

class ShaderSystem
{
public:
    void Initialize();

    SDL_GPUShader *LoadShader(SDL_GPUDevice *device,
                              const char *shaderFilename,
                              const char *entryPoint,
                              SDL_GPUShaderStage shaderStage);
};