
#include "application.h"
#include "clock.h"
#include "filesystem.h"
#include "gfx/sprite_renderer.h"
#include "gfx/glutil.h"

namespace cs
{
void Application::run(const std::vector<char*>& args)
{
    auto current_time = 0.f;
    init(args);

    /* Main Loop */
    while (m_running)
    {
        /* Pseudo Code at the moment, must capture delta time etc. */
        handle_input();
        update(get_current_time() - current_time);
        current_time += get_current_time();
        draw();
    }

    deinit();
}

void Application::handle_input()
{
    SDL_Event e{};
    while (SDL_PollEvent(&e))
    {
        if ((e.type == SDL_WINDOWEVENT && e.window.type == SDL_WINDOWEVENT_CLOSE) ||
            (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
        {
            /* TODO: Do not abort */
            m_running = false;
        }
    }
}

void Application::update(float dt)
{
}

void Application::draw()
{
    static gfx::SpriteRenderer sp_r;
    m_window.clear();
    sp_r.draw({0, 0, 0});
    m_window.display();
}

bool Application::init(std::vector<char*> args)
{
    return init_subsystem(&Application::init_gl, "OpenGL") &&           // Init OpenGL
           init_subsystem(&Application::init_imgui, "ImGui") &&         // Init ImGui
           init_subsystem(&Application::init_physfs, "PhysFS", args) && // Init PhysFS
           init_subsystem(&Application::init_lua, "Lua") &&             // Init Lua
           init_subsystem(&Application::init_input, "Input Manager");   // Init Input Manager
}

bool Application::init_gl()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        spdlog::error("failed to initialize SDL, {} ", SDL_GetError());
        return false;
    }

    if (!m_window.init())
    {
        spdlog::error("failed to initialize window");
        return false;
    };

    if (!gladLoadGL())
    {
        spdlog::error("failed to initialize glad");
        return false;
    }

#ifndef NDEBUG
    gfx::create_debug_callback();
#endif

    return true;
}

bool Application::init_imgui()
{
    // TODO: change true to false, also make the function
    return true;
}

bool Application::init_physfs(std::vector<char*> args)
{
    return fs::init(args[0]);
}

bool Application::init_lua()
{
    /* Load necessary libraries for Lua */
    m_lua.open_libraries(sol::lib::base, sol::lib::math);

    /* Bind IO Functions (globally) */
    m_lua.set_function("writeFile", fs::write_file);
    m_lua.set_function("readFile", fs::read_file);
    m_lua.set_function("makeDirectory", fs::mkdir);
    m_lua.set_function("moveFile", fs::move_file);
    m_lua.set_function("fileExists", fs::exists);
    m_lua.set_function("deleteFile", fs::delete_file);
    m_lua.set_function("copyFile", fs::copy_file);

    /* Bind Log Functions (available in log.*) */
    auto log_table = m_lua.create_table("log");
    log_table.set_function("debug", [](std::string_view msg) { spdlog::debug(msg); });
    log_table.set_function("info", [](std::string_view msg) { spdlog::info(msg); });
    log_table.set_function("warn", [](std::string_view msg) { spdlog::warn(msg); });
    log_table.set_function("error", [](std::string_view msg) { spdlog::error(msg); });
    log_table.set_function("critical", [](std::string_view msg) { spdlog::critical(msg); });

    return true;
}

bool Application::init_input()
{
    /* TODO: Fix to not return true */
    return true;
}

void Application::deinit()
{
    deinit_input();
    deinit_lua();
    deinit_physfs();
    deinit_imgui();
    deinit_gl();
}

void Application::deinit_input()
{
}

void Application::deinit_lua()
{
}

void Application::deinit_physfs()
{
    fs::deinit();
}

void Application::deinit_imgui()
{
}

void Application::deinit_gl()
{
    m_window.deinit();

    SDL_Quit();
}
} // namespace cs
