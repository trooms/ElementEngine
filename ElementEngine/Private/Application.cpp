#include <Application.h>
#include <RenderContext.h>

Application::Application()
{
    mRenderContext = nullptr;
}

Application::Application(const Application &)
{
}

Application::~Application()
{
}

bool Application::Initialize(int screenWidth, int screenHeight, SDL_Window *window)
{
    bool result{false};

    mRenderContext = new RenderContext;

    result = mRenderContext->Initialize(screenWidth, screenHeight, window);
    return result;
}

void Application::Shutdown()
{
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
    // mRenderContext->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);
    mRenderContext->EndScene();

    return true;
}