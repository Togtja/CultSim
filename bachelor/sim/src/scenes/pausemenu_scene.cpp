#include "pausemenu_scene.h"
#include "common_helpers.h"
#include "input/input_handler.h"
#include "mainmenu_scene.h"
#include "preference_scene.h"
#include "scene_manager.h"

#include <gfx/ImGUI/imgui.h>

namespace cs
{
void PauseMenuScene::on_enter()
{
    ImGui::OpenPopup("Paused##Menu");
    input::get_input().add_context(input::EKeyContext::PauseMenu, true);
}

void PauseMenuScene::on_exit()
{
    input::get_input().remove_context(input::EKeyContext::PauseMenu);
}

bool PauseMenuScene::update(float dt)
{
    ImGui::OpenPopup("Paused##Menu");
    if (ImGui::BeginPopupModal("Paused##Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::CaptureKeyboardFromApp(false);
        ImGui::Text("You have paused.");

        if (ImGui::Button("Resume", {150, 25}))
        {
            m_context->scene_manager->pop();
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button("Main Menu", {150, 25}))
        {
            m_context->scene_manager->clear();
            m_context->scene_manager->push<MainMenuScene>();
        }

        if (ImGui::Button("Key Bindings", {150, 25}))
        {
            m_context->scene_manager->push<PreferenceScene>();
        }

        if (ImGui::Button("Exit", {150, 25}))
        {
            m_context->scene_manager->clear();
        }

        ImGui::EndPopup();
    }

    return false;
}

bool PauseMenuScene::draw()
{
    return true;
}
} // namespace cs
