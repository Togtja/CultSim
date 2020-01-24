#pragma once

#include <string>

#include <SDL.h>
namespace cs
{
class Window
{
private:
    SDL_Window* m_window = nullptr;

public:
    Window();
    SDL_Window* get_window();
    bool init(std::string name = "CultSim", int width = 1280, int height = 720);

private:
};
} // namespace cs
