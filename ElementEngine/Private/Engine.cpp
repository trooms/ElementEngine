#include <Engine.h>
#include <Input.h>
#include <Application.h>

#include <SDL3/SDL.h>

Engine::Engine()
{
    mInput = nullptr;
    mApplication = nullptr;
    mWindow = nullptr;
}

Engine::Engine(const Engine &other)
{
}

Engine::~Engine()
{
}

bool Engine::Initialize()
{
    int screenWidth, screenHeight;
    bool exitCode{false};

    screenWidth = 0;
    screenHeight = 0;

    exitCode = InitializeWindow(screenWidth, screenHeight);
    if (!exitCode)
        return exitCode;

    mInput = new Input;
    mInput->Initialize();

    mApplication = new Application;
    exitCode = mApplication->Initialize(screenWidth, screenHeight, mWindow);

    return exitCode;
}

void Engine::Shutdown()
{
    if (mApplication)
    {
        mApplication->Shutdown();
        delete mApplication;
        mApplication = 0;
    }

    if (mInput)
    {
        delete mInput;
        mInput = nullptr;
    }

    ShutdownWindow();
}

void Engine::Run()
{
    // The event data
    SDL_Event e;
    SDL_zero(e);
    bool stop{false};

    while (!stop)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                stop = true;
            }
        }
        if (!stop)
        {
            mInput->UpdateKeys(SDL_GetKeyboardState(nullptr));
            stop = !Frame();
        }
    }

    return;
}

bool Engine::Frame()
{
    if (mInput->IsKeyDown(SDL_SCANCODE_ESCAPE))
    {
        return false;
    }

    return mApplication->Frame();
}

bool Engine::InitializeWindow(int &screenWidth, int &screenHeight)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return false;
    }

    SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(displayID);

    screenWidth = displayMode->w;
    screenHeight = displayMode->h - 200; // TODO

    SDL_WindowFlags windowFlags = (SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (mWindow = SDL_CreateWindow("ElementEngine", screenWidth, screenHeight, windowFlags); mWindow == nullptr)
    {
        SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetWindowPosition(
        mWindow,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED);

    SDL_ShowWindow(mWindow);

    return true;
}

void Engine::ShutdownWindow()
{
    if (mWindow)
    {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }

    SDL_Quit();
}