#pragma once

#include <Shader/Shader.h>
#include <SDL3/SDL_gpu.h>
#include <mat4x4.hpp>
#include <vec4.hpp>

class ColorShader : public Shader
{
private:
    struct MatrixBuffer
    {
        glm::mat4x4 world;
        glm::mat4x4 view;
        glm::mat4x4 projection;
    };

    struct VertexInput
    {
        glm::vec3 position;
        glm::vec4 color;
    };

public:
    void Initialize(SDL_GPUDevice *);

    void Shutdown();

    void SetShaderUniforms(SDL_GPUCommandBuffer *, glm::mat4x4, glm::mat4x4, glm::mat4x4);
    void ReleaseShaders(SDL_GPUDevice *device);

private:
    SDL_GPUVertexAttribute mVertexAttributes[2];
    SDL_GPUVertexBufferDescription mVertexBufferDescriptions[1];

    class SlangShaderSystem *mShaderSystem;
};