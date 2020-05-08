#include "loadscenario_scene.h"
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

    RandomEngine temp_rng{};
    for (unsigned i = 0; i < s_seed_length; ++i)
    {
        m_seed[i] = temp_rng.uniform('a', 'z');
    }
    m_seed[s_seed_length - 1] = '\0';
}

void LoadScenarioScene::on_exit()
{
    input::get_input().remove_context(input::EKeyContext::LoadScenario);
}

bool LoadScenarioScene::update(float dt)
{
    /** Shows the popup to select scenario */
    if (ImGui::BeginPopupModal("Select##Scenario", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::CaptureKeyboardFromApp(false);

        if (ImGui::TreeNode("Load Options"))
        {
            ImGui::TextColored({0.6, 0.6, 0.6, 1.0}, "Random Seed");
            ImGui::InputText("##SeedSlider", m_seed, s_seed_length - 1);
            ImGui::Checkbox("Enable rendering", &m_enable_rendering);
        }

        const auto& scenarios = fs::list_directory("script/scenarios/");
        for (const auto& scenario : scenarios)
        {
            if (fs::is_directory("script/scenarios/" + scenario))
            {
                if (ImGui::Button(scenario.c_str(), {280, 30}))
                {
                    m_context->scene_manager->push<ScenarioScene>(
                        fmt::format("script/scenarios/{}", scenario),
                        lua::ScenarioLoadPreferences{entt::hashed_string(m_seed).value(), m_enable_rendering});
                }
            }
        }

        /** Exit the loading screen */
        ImGui::Separator();
        if (ImGui::Button("Cancel", {280, 30}))
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
