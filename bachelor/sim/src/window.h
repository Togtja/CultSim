#pragma once

#include <string>

#include <SDL.h>
#include <glm/vec3.hpp>

namespace cs
{
class Window
{
private:
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_context;

public:
    SDL_Window* get_window();

    /**
     * init will initialize a Window with the provided title at the provided size
     *
     * @param name is the title of the window
     * @param width requested width (and resolution) of the window
     * @param length requested length (and resolution) of the window
     * @return A pointer to the newly created SDL Window, or NULL if it failed
     */
    bool init(std::string name = "CultSim", int width = 1280, int height = 720);

    void clear();

    void display();

    void set_background_color(glm::vec3 color);

private:
};
} // namespace cs
