#pragma once

#include <SDL2/SDL.h>

namespace cs
{
/**
 * @brief The Application class
 */
class Application
{
private:
    SDL_Window* m_window = nullptr;

public:
    void run();

private:
};

} // namespace cs
