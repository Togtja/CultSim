#include "pausemenu_scene.h"
#include "mainmenu_scene.h"
#include "preference_scene.h"
#include "scene_manager.h"

#include <common_helpers.h>
#include <gfx/ImGUI/imgui.h>

namespace cs
{
void PauseMenuScene::on_enter()
{
    ImGui::OpenPopup("Paused##Menu");
}

void PauseMenuScene::on_exit()
{
}

bool PauseMenuScene::update(float dt)
{
    ImGui::OpenPopup("Paused##Menu");
    if (ImGui::BeginPopupModal("Paused##Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
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
