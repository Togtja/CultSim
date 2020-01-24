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

void Application::init()
{
    init_gl();
    init_imgui();
    init_physfs();
    init_lua();
    init_entt();
    init_input();
}

void Application::init_gl()
{
}

void Application::init_imgui()
{
}

void Application::init_physfs()
{
}

void Application::init_lua()
{
}

void Application::init_entt()
{
}

void Application::init_input()
{
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
