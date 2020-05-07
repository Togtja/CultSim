#pragma once
#include "gfx/window.h"
#include "preferences.h"
#include "scenes/scene_manager.h"

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
    Window m_window{};

    /** The Lua state used throughout the application */
    sol::state m_lua{};

    /** Holds the preferences set by the user */
    PreferenceManager m_preferences{m_window, m_lua.lua_state()};

    /** Manages the current state / scene the application is in */
    SceneManager m_scene_manager{};

    /** Should the application keep running */
    bool m_running{true};

public:
    /**
     * Run the application, starting the main loop until a crash or normal shutdown
     */
    void run(const std::vector<char*>& args);

private:
    /**
     * Process application wide events and delegate the rest to an input manager
     */
    void handle_input();

    /**
     * Ticks the application at a fixed time step
     *
     * @param dt The delta time since last frame
     */
    void update(float dt);

    /**
     * Call the renderer and make it draw to screen
     */
    void draw();

    /**
     * Initialize the application and required subsystems
     *
     * @param args The program arguments
     * @return True if successful
     */
    bool init(const std::vector<char*>& args);

    /**
     * Initialize PhysFS' filesystem
     *
     * @param args Command line arguments to the program
     * @return True if successful
     */
    bool init_physfs(std::vector<char*> args);

    /**
     * Initialize key bindings and input management
     *
     * @return True if successful
     */
    bool init_input();

    /**
     * Initialize and bind most application wide Lua functions and types
     *
     * @return True if successful
     */
    bool init_lua();

    /**
     * Initialize OpenGL, SDL and the Window
     */
    bool init_gl();

    /**
     * Load the preferences
     *
     * @return True if successful
     */
    bool init_preferences();

    /**
     * Initialize ImGui so we can use it to draw Debug GUI's
     */
    bool init_imgui();

    /**
     * Shutdown all subsystems
     *
     */
    void deinit();

    /**
     * Save preferences to file and close
     */
    void deinit_preferences();

    /**
     * Shutdown the imgui context
     */
    void deinit_imgui();

    /**
     * Close the window and clean up GL state
     */
    void deinit_gl();

    /**
     * Deinitialize the Filesystem
     */
    void deinit_physfs();

    /**
     * Initialize the subsystem with one of the Application's init_ functions and log on error
     *
     * @tparam Func The member function pointer for any of the init functions in the Application class
     * @tparam Args Variadic template arguments for any remaining arguments
     * @param f The init function to use
     * @param name The name of the subsystem for use in logging
     * @param args Any function arguments the initialization function might take
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
