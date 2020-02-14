#include "window.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace cs
{
Window::Window(Window&& other) noexcept : m_window(other.m_window), m_context(other.m_context)
{
    other.m_window  = nullptr;
    other.m_context = nullptr;
}

Window& Window::operator=(Window&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    deinit();
    m_window  = other.m_window;
    m_context = other.m_context;

    other.m_window  = nullptr;
    other.m_context = nullptr;

    return *this;
}

SDL_Window* Window::get() const
{
    return m_window;
}

bool Window::init(std::string_view name, int width, int height)
{
    m_window = SDL_CreateWindow(name.data(),
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                width,
                                height,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (m_window == nullptr)
    {
        const auto error = SDL_GetError();
        spdlog::error("failed to create window: {}", error);
        return false;
    }

    /* Create a double buffered OpenGL 4.5 Core Profile context */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    m_context = SDL_GL_CreateContext(m_window);

    return true;
}

void Window::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::display()
{
    SDL_GL_SwapWindow(m_window);
}

void Window::set_background_color(glm::vec3 color)
{
    glClearColor(color.r, color.g, color.b, 0.f);
}

void Window::resize(const glm::ivec2& resolution)
{
    SDL_SetWindowSize(m_window, resolution.x, resolution.y);
}

void Window::set_fullscreen(bool fullscreen)
{
    if (fullscreen)
    {
        SDL_SetWindowBordered(m_window, SDL_FALSE);

        SDL_DisplayMode dmode{};
        SDL_GetDesktopDisplayMode(0, &dmode);

        SDL_SetWindowSize(m_window, dmode.w, dmode.h);
        SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
    else
    {
        SDL_SetWindowBordered(m_window, SDL_TRUE);
        SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
}

void Window::deinit() noexcept
{
    if (m_context)
    {
        SDL_GL_DeleteContext(m_context);
    }

    if (m_window)
    {
        SDL_DestroyWindow(m_window);
    }
}

SDL_GLContext Window::get_context() const
{
    return m_context;
}
} // namespace cs
