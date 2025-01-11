#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include <queue>
#include <functional>

class Renderer;

class Client
{
public:
    static constexpr int SCREEN_WIDTH{640};
    static constexpr int SCREEN_HEIGHT{480};

    bool Initialize();
    void Run();
    void Shutdown();

    inline SDL_Window *GetWindow() { return WINDOW; }

protected:
    static int RenderThreadFunc(void *data);
    // static int AnimationThreadFunc(void *data);

    void AddRenderTask(std::function<void()> task);

private:
    bool quit{false};

    Renderer *LocalRenderer;

    SDL_Window *WINDOW;

    SDL_Thread *renderThread = nullptr;
    // SDL_Thread *animationThread = nullptr;

    SDL_Mutex *renderMutex;
    SDL_Condition *renderCondition;

    std::queue<std::function<void()>> renderQueue;
};
