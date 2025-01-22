#include <Application.h>
#include <RenderContext.h>
#include <Camera.h>
#include <Model.h>
#include <Shader/Custom/ColorShader.h>
#include <SDL3/SDL_log.h>

Application::Application()
{
    mRenderContext = nullptr;
    mCamera = nullptr;
    mModel = nullptr;
    mColorShader = nullptr;
}

Application::Application(const Application &)
{
}

Application::~Application()
{
}

bool Application::Initialize(int screenWidth, int screenHeight, SDL_Window *window)
{
    mRenderContext = new RenderContext;
    mColorShader = new ColorShader;
    if (!mRenderContext->Initialize(screenWidth, screenHeight, window, mColorShader))
    {
        SDL_Log("Could not initialize render context");
        return false;
    }

    mCamera = new Camera;
    mCamera->SetPosition(0.0f, 0.0f, -5.0f);

    mModel = new Model;
    if (!mModel->Initialize(mRenderContext->GetDevice()))
    {
        SDL_Log("Could not initialize model");
        return false;
    }

    return true;
}

void Application::Shutdown()
{
    if (mCamera)
    {
        delete mCamera;
        mCamera = nullptr;
    }

    if (mColorShader)
    {
        mColorShader->Shutdown();
        delete mColorShader;
        mColorShader = nullptr;
    }

    if (mModel)
    {
        mModel->Shutdown(mRenderContext->GetDevice());
        delete mModel;
        mModel = nullptr;
    }

    if (mRenderContext)
    {
        mRenderContext->Shutdown();
        delete mRenderContext;
        mRenderContext = nullptr;
    }

    return;
}

bool Application::Frame()
{
    return Render();
}

bool Application::Render()
{
    mCamera->Render();

    glm::mat4x4 worldMatrix, viewMatrix, projectionMatrix;
    mRenderContext->GetWorldMatrix(worldMatrix);
    mCamera->GetViewMatrix(viewMatrix);
    mRenderContext->GetProjectionMatrix(projectionMatrix);

    RenderContext::SceneResources sceneResources;

    mRenderContext->InitScene(sceneResources);
    mColorShader->SetShaderUniforms(sceneResources.cmdbuf, worldMatrix, viewMatrix, projectionMatrix);
    mRenderContext->BeginScene(sceneResources);
    mModel->Render(sceneResources.renderPass);
    mRenderContext->EndScene(sceneResources);

    return true;
}