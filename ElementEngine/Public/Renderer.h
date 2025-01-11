#pragma once
#include <SDL3/SDL_stdinc.h>

class Client;
struct SDL_Window;
struct SDL_GPUGraphicsPipeline;
struct SDL_GPUBuffer;
struct SDL_GPUShader;
struct SDL_GPUDevice;

class Renderer
{
public:
    Renderer(Client *TargetApplication) : TargetApp(TargetApplication) {};

    bool Initialize();
    bool Draw();

private:
    Client *TargetApp = nullptr;
    SDL_GPUDevice *GPU_DEVICE = nullptr;

    SDL_GPUGraphicsPipeline *Pipeline = nullptr;
    SDL_GPUBuffer *VertexBuffer = nullptr;
    SDL_GPUBuffer *IndexBuffer = nullptr;
};

typedef struct PositionVertex
{
    float x, y, z;
} PositionVertex;

typedef struct PositionColorVertex
{
    float x, y, z;
    Uint8 r, g, b, a;
} PositionColorVertex;

typedef struct PositionTextureVertex
{
    float x, y, z;
    float u, v;
} PositionTextureVertex;

// Matrix Math
typedef struct Matrix4x4
{
    float m11, m12, m13, m14;
    float m21, m22, m23, m24;
    float m31, m32, m33, m34;
    float m41, m42, m43, m44;
} Matrix4x4;

typedef struct Vector3
{
    float x, y, z;
} Vector3;