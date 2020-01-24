#include "application.h"

namespace cs
{
void Application::run()
{
    init();

    /* Main Loop */
    while (false) {}

    deinit();
}

void Application::handle_input()
{
}

void Application::update(float dt)
{
}

void Application::draw()
{
}

bool Application::init()
{
    return init_subsystem(&Application::init_gl, "OpenGL") &&         // Init OpenGL
           init_subsystem(&Application::init_imgui, "ImGui") &&       // Init ImGui
           init_subsystem(&Application::init_physfs, "PhysFS") &&     // Init PhysFS
           init_subsystem(&Application::init_lua, "Lua") &&           // Init Lua
           init_subsystem(&Application::init_entt, "EnTT") &&         // Init EnTT
           init_subsystem(&Application::init_input, "Input Manager"); // Init Input Manager
}

bool Application::init_gl()
{
    return false;
}

bool Application::init_imgui()
{
    return false;
}

bool Application::init_physfs()
{
    return false;
}

bool Application::init_lua()
{
    return false;
}

bool Application::init_entt()
{
    return false;
}

bool Application::init_input()
{
    return false;
}

void Application::deinit()
{
    deinit_input();
    deinit_entt();
    deinit_lua();
    deinit_physfs();
    deinit_imgui();
    deinit_gl();
}

void Application::deinit_input()
{
}

void Application::deinit_entt()
{
}

void Application::deinit_lua()
{
}

void Application::deinit_physfs()
{
}

void Application::deinit_imgui()
{
}

void Application::deinit_gl()
{
}
} // namespace cs
