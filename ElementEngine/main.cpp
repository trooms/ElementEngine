#include <Engine.h>

int main(int argc, char *argv[])
{
    Engine *ElementEngine;
    bool initialized{false};

    ElementEngine = new Engine;
    initialized = ElementEngine->Initialize();
    if (initialized)
    {
        ElementEngine->Run();
    }

    ElementEngine->Shutdown();
    delete ElementEngine;
    ElementEngine = nullptr;

    return initialized;
}

/*#include <Client.h>

int main(int argc, char *argv[])
{
    int exitCode{0};

    Client ClientApp;

    if (!ClientApp.Initialize())
    {
        SDL_Log("Unable to initialize program!\n");
        exitCode = 1;
    }
    else
    {
        ClientApp.Run();
    }

    ClientApp.Shutdown();
    return exitCode;
}
*/