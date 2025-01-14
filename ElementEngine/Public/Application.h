#pragma once

class Application
{
public:
    Application();
    Application(const Application &);
    ~Application();

    bool Initialize(int, int, struct SDL_Window *);
    void Shutdown();
    bool Frame();

private:
    bool Render();

    class RenderContext *mRenderContext;
};