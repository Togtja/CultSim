#include "window.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace cs
{
SDL_Window* Window::get_window()
{
    return m_window;
}

bool Window::init(std::string name, int width, int height)
{
    m_window = SDL_CreateWindow(name.c_str(),
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                width,
                                height,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (m_window == nullptr)
    {
        spdlog::error("failed to create window");
        return false;
    }

    /* Create a double buffer context with the provided settings */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_context = SDL_GL_CreateContext(m_window);

    return true;
}

void Window::clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::display()
{
    SDL_GL_SwapWindow(m_window);
}

void Window::set_background_color(glm::vec3 color)
{
    glClearColor(color.r, color.g, color.b, 0.f);
}
} // namespace cs
