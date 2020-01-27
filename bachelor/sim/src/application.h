#pragma once
#include "window.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace cs
{
/**
 * Holds all state, the window and handles the main loop
 */
class Application
{
private:
    Window m_window;

public:
    /**
     * Run the application, starting the main loop until a crash or normal shutdown
     */
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
     * Initialize the subsystem with one of the Application's init_ functions and log on error
     *
     * @param f The init function to use
     * @param name The name of the subsystem for use in logging
     */
    template<typename Func>
    bool init_subsystem(Func&& f, const std::string& name)
    {
        if (!std::invoke(f, this))
        {
            spdlog::error("failed to initialize {}", name);
            return false;
        }
        spdlog::debug("initialized {}", name);
        return true;
    }
};

} // namespace cs
