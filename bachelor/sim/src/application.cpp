#include "application.h"
#include "constants.h"
#include "filesystem.h"
#include "gfx/glutil.h"
#include "gfx/sprite_renderer.h"
#include "inputhandler.h"
#include "l10n/lang_manager.h"

#include <chrono>
#include <functional>

#include "gfx/ImGUI/imgui.h"
#include "gfx/ImGUI/imgui_impl_opengl3.h"
#include "gfx/ImGUI/imgui_impl_sdl.h"

namespace cs
{
void Application::run(const std::vector<char*>& args)
{
    auto current_time = std::chrono::steady_clock::now();
    auto lag          = 0.f;

    init(args);
    /* Main Loop */
    while (m_running)
    {
        auto elapsed = std::chrono::duration<float>(std::chrono::steady_clock::now() - current_time).count();
        lag += elapsed;

        handle_input();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(m_window.get());
        ImGui::NewFrame();

        /** TODO: Let the frame rate be set in preferences / options menu */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(m_window.get());
        ImGui::NewFrame();

        while (lag >= SEC_PER_LOOP)
        {
            update(SEC_PER_LOOP);
            lag -= SEC_PER_LOOP;
        }

        ImGui::Text("FPS: %6.3f", 1.f / elapsed);

        current_time = std::chrono::steady_clock::now();

        draw();
    }

    deinit();
}

void Application::handle_input()
{
    SDL_Event e{};
    while (SDL_PollEvent(&e))
    {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if ((e.type == SDL_WINDOWEVENT && e.window.type == SDL_WINDOWEVENT_CLOSE) ||
            (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
        {
            m_running = false;
        }
        if (e.type == SDL_KEYDOWN)
        {
            input::get_input().handle_input(e.key.keysym.scancode);
        }
    }
}

void Application::update(float dt)
{
}

void Application::draw()
{
    static gfx::SpriteRenderer r{};
    m_window.clear();

    auto time            = SDL_GetTicks();
    constexpr float nspr = 1'000'000;
    for (int i = 0; i < std::sqrt(nspr) - 1; ++i)
    {
        auto xratio = i / std::sqrt(nspr);

        for (int j = 0; j < std::sqrt(nspr) - 1; ++j)
        {
            auto yratio = j / std::sqrt(nspr);
            r.draw({-960.f + xratio * 1920.f, -540.f + yratio * 1080.f, 0.f},
                   {std::sin(time / 1000.f) + 1.f - xratio, std::cos(time / 1000.f) + yratio, xratio},
                   {});
        }
    }

    r.display();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
    // Set up Context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Set up Style
    ImGui::StyleColorsDark();

    // Set up Platform & renderer Bindings
    ImGui_ImplSDL2_InitForOpenGL(m_window.get(), m_window.get_context());
    ImGui_ImplOpenGL3_Init("#version 450 core");

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
    input::ContextHandler& inputs = input::get_input();
    std::function<void()> test([] { spdlog::info("You have hit the spacebar"); });

    inputs.bind_key(input::KeyContext::DefaultContext, SDL_SCANCODE_SPACE, test);
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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void Application::deinit_gl()
{
    m_window.deinit();

    SDL_Quit();
}
} // namespace cs
