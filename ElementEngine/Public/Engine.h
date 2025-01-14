#pragma once

class Engine
{
public:
    Engine();
    Engine(const Engine &);
    ~Engine();

    bool Initialize();
    void Shutdown();
    void Run();

private:
    bool Frame();
    bool InitializeWindow(int &, int &);
    void ShutdownWindow();

private:
    class Input *mInput;
    class Application *mApplication;
    struct SDL_Window *mWindow;
};