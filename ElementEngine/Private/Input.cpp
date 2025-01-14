#include <Input.h>

Input::Input()
{
}
Input::Input(const Input &other)
{
}
Input::~Input()
{
}

void Input::Initialize()
{
    mKeyState = nullptr;
}

void Input::UpdateKeys(const bool *keyState)
{
    mKeyState = keyState;
}

bool Input::IsKeyDown(const SDL_Scancode &scancode)
{
    return mKeyState[scancode];
}