#include "application.h"
#include "constants.h"
#include "delta_clock.h"
#include "entity/components.h"
#include "entity/rendering.h"
#include "filesystem/filesystem.h"
#include "gfx/glutil.h"
#include "input/input_handler.h"
#include "scenes/scenario_scene.h"

#include <chrono>
#include <functional>

#include "gfx/ImGUI/imgui.h"
#include "gfx/ImGUI/imgui_impl_opengl3.h"
#include "gfx/ImGUI/imgui_impl_sdl.h"

namespace cs
{
void Application::run(const std::vector<char*>& args)
{
    init(args);

    /** Add default scene */
    m_scene_manager.push<ScenarioScene>("basic_needs");

    /** Temporary replacement of DT until we figure out frame rate issues! */
    DeltaClock dt_clock{};

    /** Main Loop */
    while (m_running)
    {
        handle_input();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(m_window.get());
        ImGui::NewFrame();

        update(dt_clock.restart());

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
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
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
    m_scene_manager.update(dt);
}

void Application::draw()
{
    m_scene_manager.draw();
    m_window.clear();

    auto& r = gfx::get_renderer();
    r.sprite().display();

    r.debug().draw_line({-100.f, 0.f, 0.f}, {100.f, 0.f, 0.f}, {1.f, 0.f, 0.f});
    r.debug().draw_line({0.f, -100.f, 0.f}, {0.f, 100.f, 0.f}, {0.f, 1.f, 0.f});
    r.debug().draw_line({0.f, 0.f, -100.f}, {0.f, 0.f, 100.f}, {0.f, 0.f, 1.f});

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    m_window.display();
}

bool Application::init(const std::vector<char*>& args)
{
    return init_subsystem(&Application::init_gl, "OpenGL") &&           // Init OpenGL
           init_subsystem(&Application::init_physfs, "PhysFS", args) && // Init PhysFS
           init_subsystem(&Application::init_imgui, "ImGui") &&         // Init ImGui
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
    }

    if (!gladLoadGL())
    {
        spdlog::error("failed to initialize glad");
        return false;
    }

    glClearColor(0.12f, 0.12f, 0.12f, 0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

#ifndef NDEBUG
    gfx::create_debug_callback();
#endif

    return true;
}

bool Application::init_physfs(std::vector<char*> args)
{
    return fs::init(args[0]);
}

bool Application::init_imgui()
{
    /** Set up Context */
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    /** Load custom UI Font */
    auto font_file = fs::read_byte_file("fonts/CenturyGothicGras700.ttf");

    /** Malloc because ImGui uses it under the hood, and will take ownership */
    auto font_memory = malloc(font_file.size());
    memcpy(font_memory, font_file.data(), font_file.size());

    /** Give font ot ImGui (do not free above memory! ImGui does it) */
    io.Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(font_memory), 14, 14);

    /** Set up Style colors */
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
    colors[ImGuiCol_PlotLines]             = ImVec4(0.87f, 0.87f, 0.87f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.51f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.52f, 0.26f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 0.67f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]          = ImVec4(0.98f, 0.67f, 0.26f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    // Set up Platform & renderer Bindings
    ImGui_ImplSDL2_InitForOpenGL(m_window.get(), m_window.get_context());
    ImGui_ImplOpenGL3_Init("#version 450 core");

    return true;
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
    std::function<void()> test([] { spdlog::info("You have hit the space bar"); });

    inputs.bind_key(input::KeyContext::DefaultContext, SDL_SCANCODE_W, [] { gfx::get_renderer().move_camera({0.f, 1.f, 0.f}); });
    inputs.bind_key(input::KeyContext::DefaultContext, SDL_SCANCODE_A, [] { gfx::get_renderer().move_camera({-1.f, 0.f, 0.f}); });
    inputs.bind_key(input::KeyContext::DefaultContext, SDL_SCANCODE_S, [] { gfx::get_renderer().move_camera({0.f, -1.f, 0.f}); });
    inputs.bind_key(input::KeyContext::DefaultContext, SDL_SCANCODE_D, [] { gfx::get_renderer().move_camera({1.f, 0.f, 0.f}); });
    inputs.bind_key(input::KeyContext::DefaultContext, SDL_SCANCODE_Q, [] { gfx::get_renderer().move_camera({0.f, 0.f, -4.f}); });
    inputs.bind_key(input::KeyContext::DefaultContext, SDL_SCANCODE_E, [] { gfx::get_renderer().move_camera({0.f, 0.f, 4.f}); });
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
