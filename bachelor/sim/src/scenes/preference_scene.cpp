#include "preference_scene.h"
#include "preferences.h"
#include "scene_manager.h"

#include <common_helpers.h>
#include <gfx/ImGUI/imgui.h>
#include <spdlog/spdlog.h>

namespace cs
{
void PreferenceScene::on_enter()
{
}

void PreferenceScene::on_exit()
{
}

bool PreferenceScene::update(float dt)
{
    key_binding();
    auto res = std::get<glm::ivec2>(m_context->preferences->get_resolution().value);
    ImGui::SetNextWindowPos(glm::vec2(res.x / 2, res.y / 1.1f), 0, {.5f, .5f});
    ImGui::Begin("Option##quit",
                 nullptr,
                 ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
    bool success = true;
    if (ImGui::Button("Save"))
    {
        auto&& input = input::get_input();
        for (auto&& [context, action_h] : m_key_map)
        {
            for (auto&& [key, action] : action_h.get_key_binding())
            {
                input.unbind_key(context, key);
                input.bind_key(context, key, action);
            }
        }
        m_context->scene_manager->pop();
    }

    ImGui::End();
    return false;
}

bool PreferenceScene::draw()
{
    return false;
}

void PreferenceScene::key_binding()
{
    ImGui::Begin("Key Bindings", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    for (auto&& [context, action_h] : m_key_map)
    {
        ImGui::Text(input::key_context_to_string(context).c_str());
        ImGui::BeginTable(input::key_context_to_string(context).c_str(), 3);
        ImGui::TableSetupColumn("Key");
        ImGui::TableSetupColumn("Action");
        ImGui::TableSetupColumn("");
        ImGui::TableAutoHeaders();
        int i = 0;
        for (auto&& [key, action] : action_h.get_key_binding())
        {
            ImGui::TableNextCell();
            if (ImGui::Button(SDL_GetScancodeName(key)))
            {
                m_bind_key    = key;
                m_bind_action = action;
                ImGui::OpenPopup(fmt::format("Change Binding##{}", context).c_str());
            }
            ImGui::TableNextCell();
            auto action_s = input::action_to_string(action);
            ImGui::Text(action_s.c_str());

            ImGui::TableNextCell();
            if (ImGui::Button(fmt::format("X##{}", i).c_str()))
            {
                action_h.unbind_key(key);
            }
        }

        ImGui::NewLine();
        // TODO: Do this for mouse bindings
        ImGui::TableNextCell();
        if (ImGui::Button("Add new Binding"))
        {
            ImGui::OpenPopup(fmt::format("Select Action##{}", context).c_str());
        }
        bool open_up = false;
        if (ImGui::BeginPopupModal(fmt::format("Select Action##{}", context).c_str(),
                                   nullptr,
                                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
        {
            for (int i = static_cast<int>(input::EAction::None) + 1; i < static_cast<int>(input::EAction::Count); i++)
            {
                if (ImGui::Button(input::action_to_string(static_cast<input::EAction>(i)).c_str()))
                {
                    m_bind_action = static_cast<input::EAction>(i);
                    ImGui::CloseCurrentPopup();

                    open_up = true;
                }
            }

            ImGui::EndPopup();
        }

        if (open_up)
        {
            ImGui::OpenPopup(fmt::format("Change Binding##{}", context).c_str());
            open_up = false;
        }

        if (ImGui::BeginPopupModal(fmt::format("Change Binding##{}", context).c_str(),
                                   nullptr,
                                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
        {
            ImGui::Text("Press a Key");
            input::EMouse bind_mouse = input::EMouse::None;
            SDL_Scancode n_key       = SDL_SCANCODE_UNKNOWN;
            SDL_Event e{};
            if (SDL_WaitEvent(&e))
            {
                if (e.type == SDL_KEYDOWN)
                {
                    n_key = e.key.keysym.scancode;
                }
                else if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    bind_mouse = static_cast<input::EMouse>(e.button.button);
                }
            }
            if (n_key != SDL_SCANCODE_UNKNOWN)
            {
                action_h.unbind_key(n_key);
                action_h.unbind_key(m_bind_key);
                action_h.bind_key(n_key, m_bind_action);
                m_bind_key    = SDL_SCANCODE_UNKNOWN;
                m_bind_action = input::EAction::None;
                ImGui::CloseCurrentPopup();
            }
            else if (bind_mouse != input::EMouse::None)
            {
                action_h.bind_btn(bind_mouse, m_bind_action);
                m_bind_key    = SDL_SCANCODE_UNKNOWN;
                m_bind_action = input::EAction::None;

                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::EndTable();
        ImGui::NewLine();
    }

    ImGui::End();
}

} // namespace cs
