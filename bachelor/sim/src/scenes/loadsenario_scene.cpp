#include "loadsenario_scene.h"
#include "common_helpers.h"
#include "filesystem/filesystem.h"
#include "input/input_handler.h"
#include "scenario_scene.h"
#include "scene_manager.h"

#include <gfx/ImGUI/imgui.h>
#include <spdlog/spdlog.h>

namespace cs
{
void LoadScenarioScene::on_enter()
{
    ImGui::OpenPopup("Select##Scenario");
    input::get_input().add_context(input::EKeyContext::LoadScenario, true);
}

void LoadScenarioScene::on_exit()
{
    input::get_input().remove_context(input::EKeyContext::LoadScenario);
}

bool LoadScenarioScene::update(float dt)
{
    ImGui::OpenPopup("Select##Scenario");
    /** Shows the popup to select scenario (TODO: Lua-fi) */
    if (ImGui::BeginPopupModal("Select##Scenario", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::CaptureKeyboardFromApp(false);
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
            m_context->scene_manager->pop();
        }
        ImGui::EndPopup();
    }
    return false;
}

bool LoadScenarioScene::draw()
{
    return false;
}
} // namespace cs
