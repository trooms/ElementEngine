#pragma once
#include <SDL3/SDL_scancode.h>

class Input
{
public:
    Input();
    Input(const Input &);
    ~Input();

    void Initialize();

    void UpdateKeys(const bool *);
    bool IsKeyDown(const SDL_Scancode &);

private:
    const bool *mKeyState;
};