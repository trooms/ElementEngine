#include <Client.h>
#include <Renderer.h>

bool Client::Initialize()
{
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return false;
    }
    // Create window
    if (WINDOW = SDL_CreateWindow("ElementEngine", SCREEN_WIDTH, SCREEN_HEIGHT, 0); WINDOW == nullptr)
    {
        SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
        return false;
    }

    LocalRenderer = new Renderer(this);
    LocalRenderer->Initialize();

    renderMutex = SDL_CreateMutex();
    renderCondition = SDL_CreateCondition();

    renderThread = SDL_CreateThread(RenderThreadFunc, "RenderThread", this);
    // animationThread = SDL_CreateThread(AnimationThreadFunc, "AnimationThread", this);

    return true;
}

void Client::Run()
{
    // The event data
    SDL_Event e;
    SDL_zero(e);

    Uint32 startTicks, elapsedTicks;
    const Uint32 RUN_INTERVAL = 16; // 60hz

    while (quit == false)
    {
        startTicks = SDL_GetTicks();
        // process input
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                quit = true;
            }
        }
        // process logic updates
        // TODO: Make this only occur if an object has changed
        AddRenderTask([this]()
                      { LocalRenderer->Draw(); });

        elapsedTicks = SDL_GetTicks() - startTicks;
        if (elapsedTicks < RUN_INTERVAL)
        {
            SDL_Delay(RUN_INTERVAL - elapsedTicks);
        }
    }

    // Thread exit
    SDL_LockMutex(renderMutex);
    SDL_BroadcastCondition(renderCondition);
    SDL_UnlockMutex(renderMutex);

    SDL_WaitThread(renderThread, nullptr);
    // SDL_WaitThread(animationThread, nullptr);
}

int Client::RenderThreadFunc(void *data)
{
    Client *client = static_cast<Client *>(data);

    while (!client->quit)
    {
        std::function<void()> task;

        SDL_LockMutex(client->renderMutex);
        while (client->renderQueue.empty() && !client->quit)
        {
            SDL_WaitCondition(client->renderCondition, client->renderMutex);
        }

        if (client->quit)
        {
            SDL_UnlockMutex(client->renderMutex);
            break;
        }

        task = std::move(client->renderQueue.front());
        client->renderQueue.pop();
        SDL_UnlockMutex(client->renderMutex);

        if (task)
        {
            task();
        }
    }

    return 0;
}

void Client::AddRenderTask(std::function<void()> task)
{
    SDL_LockMutex(renderMutex);
    renderQueue.push(std::move(task));
    SDL_SignalCondition(renderCondition);
    SDL_UnlockMutex(renderMutex);
}

void Client::Shutdown()
{
    SDL_DestroyWindow(WINDOW);
    WINDOW = nullptr;

    SDL_Quit();
}