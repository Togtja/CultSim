#pragma once

#include <string_view>

#include <SDL.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace cs
{
/**
 * A single window with it's own OpenGL context to use with rendering
 *
 * @note Always call init before using
 */
class Window
{
private:
    SDL_Window* m_window    = nullptr;
    SDL_GLContext m_context = nullptr;

public:
    Window& operator=(const Window& other) = delete;
    Window(const Window& other)            = delete;
    Window()                               = default;
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    /**
     * Get a pointer to the underlying SDL_Window
     *
     * @return Pointer to the underlying SDL_Window, it could be nullptr if window is not initialized     */
    [[nodiscard]] SDL_Window* get() const;

    /**
     * Initialize a Window with the provided title and size
     *
     * This function must be called after constructing an instance of this class in order to have a valid window object
     * If you don't, then you must not use it.
     *
     * @param name The title of the window
     * @param width Requested width of the window
     * @param height Requested height of the window
     * @return true if the window was successfully initialized, false otherwise
     */
    bool init(std::string_view name = "CultSim", int width = 1280, int height = 720);

    /**
     * Clear the window of all it's contents
     */
    void clear();

    /**
     * Shows the contents that have been drawn to the OpenGL backbuffer since the last call to display
     */
    void display();

    /**
     * Change the background color of this window
     *
     * @param color The color that will become the background of this window
     */
    void set_background_color(glm::vec3 color);

    void resize(const glm::ivec2& resolution);

    void set_fullscreen(bool fullscreen);

    /**
     * Clean up and destroy all underlying resources that are successfully created
     *
     * @note This is a no-op if the window is not successfully initialized
     */
    void deinit() noexcept;

    [[nodiscard]] SDL_GLContext get_context() const;

private:
};
} // namespace cs
