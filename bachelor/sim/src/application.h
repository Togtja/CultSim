#pragma once

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

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

    bool init();

    bool init_gl();

    bool init_imgui();

    bool init_physfs();

    bool init_lua();

    bool init_entt();

    bool init_input();

    void deinit();

    void deinit_input();

    void deinit_entt();

    void deinit_lua();

    void deinit_physfs();

    void deinit_imgui();

    void deinit_gl();

    /**
     * @brief init_subsystem
     */
    template<typename Func>
    bool init_subsystem(Func&& f, const std::string& name)
    {
        if (!std::invoke(f, this))
        {
            spdlog::error("failed to initialize {}", name);
            return false;
        }
        return true;
    }
};

} // namespace cs
