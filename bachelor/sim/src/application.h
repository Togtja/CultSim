#pragma once
#include "window.h"

#include <vector>

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <glad/glad.h>
#include <physfs.h>
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace cs
{
/**
 * Holds all state, the window and handles the main loop
 */
class Application
{
private:
    /**
     * @warning Do not use until initialized
     */
    Window m_window;

    sol::state m_lua{};

    entt::registry m_entt{};

    entt::dispatcher m_event_bus{};

public:
    /**
     * Run the application, starting the main loop until a crash or normal shutdown
     */
    void run(const std::vector<char*>& args);

private:
    void handle_input();

    void update(float dt);

    void draw();

    bool init(std::vector<char*> args);

    bool init_gl();

    bool init_imgui();

    bool init_physfs(std::vector<char*> args);

    bool init_lua();

    bool init_input();

    void deinit();

    void deinit_input();

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
    template<typename Func, typename... Args>
    bool init_subsystem(Func&& f, const std::string& name, Args&&... args)
    {
        if (!std::invoke(f, this, args...))
        {
            spdlog::error("failed to initialize {}", name);
            return false;
        }
        spdlog::debug("initialized {}", name);
        return true;
    }
};

} // namespace cs
