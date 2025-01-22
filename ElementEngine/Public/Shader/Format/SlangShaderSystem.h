#pragma once

#include <SDL3/SDL_gpu.h>
#include <slang.h>
#include <slang-com-ptr.h>

class SlangShaderSystem
{
public:
    void Initialize();

    SDL_GPUShader *LoadShader(SDL_GPUDevice *device,
                              const char *shaderFilename,
                              const char *entryPoint,
                              SDL_GPUShaderStage shaderStage);

    Slang::ComPtr<slang::IGlobalSession> slangSession;
};