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
    void handle_input();

    void update(float dt);

    void draw();

    void init();

    void init_gl();

    void init_imgui();

    void init_physfs();

    void init_lua();

    void init_entt();

    void init_input();

    void deinit();

    void deinit_input();

    void deinit_entt();

    void deinit_lua();

    void deinit_physfs();

    void deinit_imgui();

    void deinit_gl();
};

} // namespace cs
