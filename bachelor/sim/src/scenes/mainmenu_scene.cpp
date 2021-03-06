#include "mainmenu_scene.h"
#include "common_helpers.h"
#include "editor_scene.h"
#include "entity/scenario.h"
#include "loadscenario_scene.h"
#include "preference_scene.h"
#include "preferences.h"
#include "scenario_scene.h"
#include "scene_manager.h"

#include <gfx/ImGUI/imgui.h>

namespace cs
{
void MainMenuScene::on_enter()
{
    m_window_size    = std::get<glm::ivec2>(m_context->preferences->get_resolution().value);
    m_main_menu_size = m_window_size / 2;
    m_context->preferences->on_preference_changed.connect<&MainMenuScene::handle_size_changed>(this);
}

void MainMenuScene::on_exit()
{
    m_context->preferences->on_preference_changed.disconnect<&MainMenuScene::handle_size_changed>(this);
}

bool MainMenuScene::update(float dt)
{
    ImGui::SetNextWindowPos(m_main_menu_size, 0, {.5f, .5f});
    ImGui::SetNextWindowSize({160, 250});
    ImGui::Begin("##MainMenu", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    if (ImGui::Button("Load Scenario", {150, 50}))
    {
        m_context->scene_manager->push<LoadScenarioScene>();
    }

    if (ImGui::Button("Key Bindings", {150, 50}))
    {
        m_context->scene_manager->push<PreferenceScene>();
    }

    if (ImGui::Button("Exit", {150, 50}))
    {
        m_context->scene_manager->clear();
    }

    ImGui::End();

    return false;
}

bool MainMenuScene::draw()
{
    return false;
}

void MainMenuScene::handle_size_changed(const Preference& before, const Preference& after)
{
    if (before.name == "Resolution")
    {
        m_window_size    = std::get<glm::ivec2>(after.value);
        m_main_menu_size = m_window_size / 2;
    }
}
} // namespace cs
