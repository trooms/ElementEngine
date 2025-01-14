#pragma once
#include <vec3.hpp>
#include <vec4.hpp>

class Model
{
private:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec4 color;
    };

public:
    Model();
    Model(const Model &);
    ~Model();

    bool
    Initialize(struct SDL_GPUDevice *);
    void Shutdown(struct SDL_GPUDevice *);
    void Render(struct SDL_GPURenderPass *);

    inline int GetIndexCount() { return mNumIndices; };

private:
    bool InitializeBuffers(SDL_GPUDevice *);
    void ShutdownBuffers(SDL_GPUDevice *);
    void RenderBuffers(struct SDL_GPURenderPass *);

private:
    struct SDL_GPUBuffer *mVertexBuffer, *mIndexBuffer;
    int mNumVertices, mNumIndices;
};