
#include "application.h"
#include "constants.h"
#include "filesystem.h"
#include "gfx/glutil.h"
#include "gfx/sprite_renderer.h"

#include <chrono>

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

        /** TODO: Let the frame rate be set in preferences / options menu */
        while (lag >= SEC_PER_LOOP)
        {
            update(SEC_PER_LOOP);
            lag -= SEC_PER_LOOP;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(m_window.get());
        ImGui::NewFrame();
        {
            static int counter = 0;
            // get the window size as a base for calculating widgets geometry
            int sdl_width = 0, sdl_height = 0, controls_width = 0;
            SDL_GetWindowSize(m_window.get(), &sdl_width, &sdl_height);
            controls_width = sdl_width;
            // make controls widget width to be 1/3 of the main window width
            if ((controls_width /= 3) < 300)
            {
                controls_width = 300;
            }

            // position the controls widget in the top-right corner with some margin
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
            // here we set the calculated width and also make the height to be
            // be the height of the main window also with some margin
            ImGui::SetNextWindowSize(ImVec2(static_cast<float>(controls_width), static_cast<float>(sdl_height - 20)),
                                     ImGuiCond_Always);
            // create a window and append into it
            ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoResize);

            ImGui::Dummy(ImVec2(0.0f, 1.0f));
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Platform");
            ImGui::Text("%s", SDL_GetPlatform());
            ImGui::Text("CPU cores: %d", SDL_GetCPUCount());
            ImGui::Text("RAM: %.2f GB", SDL_GetSystemRAM() / 1024.0f);

            // buttons and most other widgets return true when clicked/edited/activated
            if (ImGui::Button("Counter button"))
            {
                std::cout << "counter button clicked\n";
                counter++;
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::End();
        }

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
    m_window.clear();

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
    ImGui_ImplOpenGL3_Init("#version 450");

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
