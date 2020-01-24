#include "window.h"

#include <spdlog/spdlog.h>

namespace cs
{
SDL_Window* Window::get_window()
{
    return m_window;
}

Window::Window()
{
}

bool Window::init(std::string name, int width, int height)
{
    m_window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    if (m_window == nullptr)
    {
        spdlog::error("failed to create window");
        return false;
    }
    return true;
}
} // namespace cs
