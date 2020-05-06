#include "application.h"
#include "constants.h"
#include "debug/auto_timer.h"
#include "delta_clock.h"
#include "entity/reflection.h"
#include "entity/systems/rendering.h"
#include "filesystem/filesystem.h"
#include "gfx/glutil.h"
#include "gfx/renderer.h"
#include "input/input_handler.h"
#include "lua_type_bindings.h"
#include "scenes/mainmenu_scene.h"
#include "scenes/pausemenu_scene.h"

#include <functional>
#include <numeric>

#include <gfx/ImGUI/imgui.h>
#include <gfx/ImGUI/imgui_impl_opengl3.h>
#include <gfx/ImGUI/imgui_impl_sdl.h>

/** Header font for ImGui purposes */
ImFont* g_header_font{nullptr};
ImFont* g_light_font{nullptr};

namespace cs
{
void Application::run(const std::vector<char*>& args)
{
    init(args);

    /** Create application context for scenes */
    ApplicationContext context = {m_lua.lua_state(), &m_scene_manager, &m_preferences};
    m_scene_manager.set_application_context(context);

    /** Add default scene */
    m_scene_manager.push<MainMenuScene>();

    /** Fixed timestep set-up */
    DeltaClock dt_clock{};
    constexpr auto timestep            = DeltaClock::TimeUnit{1.f / 60.f};
    auto time_since_tick               = timestep;
    std::array<float, 144> average_fps = {};
    int next_fps                       = 0;

    /** Main Loop */
    do
    {
        handle_input();

        auto frame_time                 = dt_clock.restart_time_unit();
        average_fps[(++next_fps) % 144] = frame_time.count();
        time_since_tick += frame_time;

        /** Update until we caught up, or only when needed otherwise */
        while (time_since_tick >= timestep)
        {
            CS_AUTOTIMER(Update Time);
            gfx::get_renderer().debug().clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(m_window.get());
            ImGui::NewFrame();
            float average = std::accumulate(average_fps.cbegin(), average_fps.cend(), 0.f) / average_fps.size();
            ImGui::Text("FPS: %.3fms / %.3f", average * 1000.f, 1.f / average);

            update(timestep.count());
            time_since_tick -= timestep;
            ImGui::Render();
        }

        draw();
    } while (m_running && !m_scene_manager.empty());

    deinit();
}

void Application::handle_input()
{
    SDL_Event e{};
    while (SDL_PollEvent(&e))
    {
        ImGui_ImplSDL2_ProcessEvent(&e);

        if (e.type == SDL_QUIT)
        {
            if (m_window.confirm_dialog("Quit?", "Do you really want to quit?"))
            {
                m_running = false;
            }
        }
        input::get_input().handle_input(e);
    }
}

void Application::update(float dt)
{
    input::get_input().handle_live_input(dt);
    ImGui::Text("Lua memory: %.2f Kb", m_lua.memory_used() / 1024.f);
    m_scene_manager.update(dt);
    m_preferences.show_ui();
    AutoTimer::show_debug_ui();
}

void Application::draw()
{
    gfx::get_renderer().raymarch().clear();
    gfx::get_renderer().sprite().clear();

    m_scene_manager.draw();

    gfx::get_renderer().display();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    m_window.display();
    m_window.clear();
}

bool Application::init(const std::vector<char*>& args)
{
    return init_subsystem(&Application::init_physfs, "PhysFS", args) &&     // Init PhysFS
           init_subsystem(&Application::init_lua, "Lua") &&                 // Init Lua
           init_subsystem(&Application::init_input, "Input Manager") &&     // Init Input Manager
           init_subsystem(&Application::init_gl, "OpenGL") &&               // Init OpenGL
           init_subsystem(&Application::init_preferences, "Preferences") && // Init Preferences
           init_subsystem(&Application::init_imgui, "ImGui");               // Init ImGui
}

bool Application::init_physfs(std::vector<char*> args)
{
    return fs::init(args[0]);
}

bool Application::init_input()
{
    input::ContextHandler& inputs = input::get_input();

    /** Load the bindings from a keybinding preference file */
    inputs.load_binding_from_file(m_lua.lua_state());
    inputs.fast_bind_key(input::EKeyContext::DefaultContext, SDL_SCANCODE_ESCAPE, input::EAction::Quit, [this] {
        m_running = false;
    });

    for (int i = static_cast<int>(input::EKeyContext::None) + 1; i < static_cast<int>(input::EKeyContext::Count); i++)
    {
        input::get_input().bind_action(static_cast<input::EKeyContext>(i), input::EAction::EscapeScene, [this] {
            m_scene_manager.pop();
        });
    }

    return true;
}

bool Application::init_lua()
{
    /* Load necessary libraries for Lua */
    m_lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table);
    m_lua.set_exception_handler(&lua::exception_handler);

    /* Bind IO Functions (globally) */
    m_lua.set_function("write_file", fs::write_file);
    m_lua.set_function("read_file", fs::read_file);
    m_lua.set_function("make_directory", fs::mkdir);
    m_lua.set_function("move_file", fs::move_file);
    m_lua.set_function("file_exists", fs::exists);
    m_lua.set_function("delete_file", fs::delete_file);
    m_lua.set_function("copy_file", fs::copy_file);

    /* Bind Log Functions (available in log.*) */
    auto log_table = m_lua.create_table("log");
    log_table.set_function("debug", [](std::string_view msg) { spdlog::get("lua")->debug(msg); });
    log_table.set_function("info", [](std::string_view msg) { spdlog::get("lua")->info(msg); });
    log_table.set_function("warn", [](std::string_view msg) { spdlog::get("lua")->warn(msg); });
    log_table.set_function("error", [](std::string_view msg) { spdlog::get("lua")->error(msg); });
    log_table.set_function("critical", [](std::string_view msg) { spdlog::get("lua")->critical(msg); });

    lua::bind_dataonly(m_lua.lua_state());
    lua::bind_components(m_lua.lua_state());
    lua::bind_systems(m_lua.lua_state());
    lua::bind_input(m_lua.lua_state());
    lua::bind_utils(m_lua.lua_state());

    /** Load lua libraries */
    for (const auto& lib : fs::list_directory("script/lib"))
    {
        spdlog::get("lua")->info("loading lua library '{}'", lib.substr(0, lib.size() - 4));
        const auto& code = fs::read_file("script/lib/" + lib);
        m_lua.require_script(lib.substr(0, lib.size() - 4), code);
    }

    meta::reflect_data_types();
    meta::reflect_systems();

    return true;
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
    }

    if (!gladLoadGL())
    {
        spdlog::error("failed to initialize glad");
        return false;
    }

    glClearColor(0.02f, 0.02f, 0.02f, 0.0f);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#ifndef NDEBUG
    gfx::create_debug_callback();
#endif

    return true;
}

bool Application::init_preferences()
{
    m_preferences.init();
    return true;
}

bool Application::init_imgui()
{
    /** Set up Context */
    ImGui::CreateContext();
    ImGuiIO& io               = ImGui::GetIO();
    io.ConfigDockingWithShift = true;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    /** Load custom UI Font */
    auto font_file       = fs::read_byte_file("fonts/CenturyGothicGras700.ttf");
    auto light_font_file = fs::read_byte_file("fonts/CenturyGothicRegular400.ttf");

    /** Malloc because ImGui uses it under the hood, and will take ownership */
    auto font_memory   = malloc(font_file.size());
    auto font_memory_2 = malloc(font_file.size());
    auto font_memory_3 = malloc(light_font_file.size());
    memcpy(font_memory, font_file.data(), font_file.size());
    memcpy(font_memory_2, font_file.data(), font_file.size());
    memcpy(font_memory_3, light_font_file.data(), light_font_file.size());

    /** Give font to ImGui (do not free above memory! ImGui does it) */
    io.Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(font_memory), 14, 14);
    g_header_font = io.Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(font_memory_2), 20, 20);
    g_light_font  = io.Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(font_memory_3), 16, 16);
    io.Fonts->Build();

    /** Set up Style colors */
    ImGui::GetStyle().WindowRounding       = 0.f;
    ImVec4* colors                         = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                  = ImVec4(0.91f, 0.91f, 0.91f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.38f, 0.38f, 0.38f, 0.59f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.06f, 0.01f, 0.12f, 0.78f);
    colors[ImGuiCol_ChildBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                = ImVec4(0.27f, 0.11f, 0.22f, 0.21f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]               = ImVec4(0.45f, 0.45f, 0.45f, 0.00f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.53f, 0.53f, 0.53f, 0.40f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.45f, 0.22f, 0.37f, 0.67f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.21f, 0.00f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.93f, 0.66f, 0.16f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(1.00f, 0.55f, 0.22f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.98f, 0.72f, 0.26f, 1.00f);
    colors[ImGuiCol_Button]                = ImVec4(0.91f, 0.25f, 0.41f, 0.40f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.98f, 0.69f, 0.26f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(1.00f, 0.60f, 0.21f, 1.00f);
    colors[ImGuiCol_Header]                = ImVec4(0.98f, 0.64f, 0.26f, 0.31f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.98f, 0.77f, 0.26f, 0.80f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(1.00f, 0.90f, 0.33f, 1.00f);
    colors[ImGuiCol_Separator]             = ImVec4(0.50f, 0.43f, 0.48f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.54f, 0.12f, 0.48f, 0.78f);
    colors[ImGuiCol_SeparatorActive]       = ImVec4(0.67f, 0.28f, 0.52f, 1.00f);
    colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.28f);
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4(1.00f, 1.00f, 1.00f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]      = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.21f, 0.05f, 0.15f, 0.86f);
    colors[ImGuiCol_TabHovered]            = ImVec4(1.00f, 0.76f, 0.26f, 0.80f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.92f, 0.59f, 0.00f, 1.00f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.11f, 0.06f, 0.07f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.42f, 0.14f, 0.29f, 1.00f);
    colors[ImGuiCol_PlotLines]             = ImVec4(0.19f, 0.86f, 0.51f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.51f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.52f, 0.26f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 0.67f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]          = ImVec4(0.98f, 0.67f, 0.26f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.39f, 0.10f, 0.35f, 0.78f);

    /** Set up Platform & renderer Bindings */
    ImGui_ImplSDL2_InitForOpenGL(m_window.get(), m_window.get_context());
    ImGui_ImplOpenGL45_Init();

    return true;
}

void Application::deinit()
{
    m_scene_manager.clear();
    input::get_input().save_binding_to_file();
    deinit_preferences();
    deinit_imgui();
    deinit_gl();
    deinit_physfs();
}

void Application::deinit_preferences()
{
    m_preferences.deinit();
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

void Application::deinit_physfs()
{
    fs::deinit();
}
} // namespace cs
