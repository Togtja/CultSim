#include "window.h"
#include "glutil.h"
#include "filesystem/filesystem.h"

#include <array>

#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace cs
{
static const SDL_MessageBoxColorScheme s_dialog_color_scheme = {
    {{15, 15, 25}, {240, 190, 0}, {45, 45, 50}, {25, 25, 35}, {255, 255, 255}}};

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

    /** Set window icon (hard coded to  our icon */
    auto pixels = gfx::load_texture("sprites/logo.png", false);
    SDL_Surface* surf =
        SDL_CreateRGBSurfaceFrom(pixels.pixels.data(), 512, 512, 32, 4 * 512, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    SDL_SetWindowIcon(m_window, surf);
    SDL_FreeSurface(surf);

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
        SDL_SetWindowSize(m_window, 1280, 720);
    }
}

bool Window::confirm_dialog(std::string_view title, std::string_view message)
{
    const SDL_MessageBoxButtonData buttons[] = {{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Confirm"},
                                                {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "Cancel"}};

    const SDL_MessageBoxData information =
        {SDL_MESSAGEBOX_WARNING, m_window, title.data(), message.data(), SDL_arraysize(buttons), buttons, &s_dialog_color_scheme};

    int response = -1;
    if (SDL_ShowMessageBox(&information, &response) < 0)
    {
        return false;
    }

    spdlog::debug("user clicked button {} - {}", response, buttons[response].text);
    return response == 0;
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
