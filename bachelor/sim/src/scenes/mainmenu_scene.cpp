#include "mainmenu_scene.h"
#include "editor_scene.h"
#include "entity/scenario.h"
#include "preference_scene.h"
#include "preferences.h"
#include "scenario_scene.h"
#include "scene_manager.h"

#include <common_helpers.h>
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
        ImGui::OpenPopup("Select##Scenario");
    }

    if (ImGui::Button("Preferences", {150, 50}))
    {
        m_context->scene_manager->push<PreferenceScene>();
    }

    if (ImGui::Button("Development Editor", {150, 50}))
    {
        m_context->scene_manager->push<EditorScene>();
    }

    if (ImGui::Button("Exit", {150, 50}))
    {
        m_context->scene_manager->clear();
    }

    /** Shows the popup to select scenario (TODO: Lua-fi) */
    if (ImGui::BeginPopupModal("Select##Scenario", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
        auto&& scenarios = fs::list_directory("script/scenarios/");
        for (auto&& scenario : scenarios)
        {
            if (fs::is_directory("script/scenarios/" + scenario))
            {
                if (ImGui::Button(scenario.c_str(), {150, 50}))
                {
                    m_context->scene_manager->push<ScenarioScene>(fmt::format("script/scenarios/{}", scenario));
                }
            }
        }
        if (ImGui::Button("Cancel", {150, 25}))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
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
