#include "preference_scene.h"
#include "preferences.h"
#include "scene_manager.h"

#include <common_helpers.h>
#include <gfx/ImGUI/imgui.h>
#include <spdlog/spdlog.h>

namespace cs
{
PreferenceScene::PreferenceScene()
{
    m_key_map = input::get_input().get_input_map();
    for (auto&& [context, action_h] : m_key_map)
    {
        m_display_map[context];
        for (auto&& [key, action] : action_h.get_key_binding())
        {
            m_display_map[context].emplace_back(std::pair(SDL_GetScancodeName(key), action));
        }
        for (auto&& [btn, action] : action_h.get_mouse_binding())
        {
            m_display_map[context].emplace_back(std::pair(input::mouse_to_string(btn), action));
        }
    }
}
void PreferenceScene::on_enter()
{
}

void PreferenceScene::on_exit()
{
}

bool PreferenceScene::update(float dt)
{
    key_binding();
    quit_btn();
    return false;
}

bool PreferenceScene::draw()
{
    return false;
}

void PreferenceScene::key_binding()
{
    // Set window size and position
    auto res = std::get<glm::ivec2>(m_context->preferences->get_resolution().value);
    ImGui::SetNextWindowPos(glm::vec2(res.x / 2.f, res.y / 2.f - 100), 0, {.5f, .5f});
    ImGui::SetNextWindowSize({res.x / 1.5f, res.y / 1.5f});

    ImGui::Begin("Key Bindings", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    for (auto&& [context, action_h] : m_display_map)
    {
        ImGui::Text(input::key_context_to_string(context).c_str());
        ImGui::BeginTable(input::key_context_to_string(context).c_str(), 3);
        ImGui::TableSetupColumn("Key");
        ImGui::TableSetupColumn("Action");
        ImGui::TableSetupColumn("");
        ImGui::TableAutoHeaders();

        int i = 0;
        for (auto&& [key, action] : action_h)
        {
            ImGui::TableNextCell();
            if (ImGui::Button(key.c_str()))
            {
                m_bind_index = i;
                ImGui::OpenPopup(fmt::format("Change Binding##{}", context).c_str());
            }
            ImGui::TableNextCell();
            auto action_s = input::action_to_string(action);
            ImGui::Text(action_s.c_str());

            ImGui::TableNextCell();
            if (ImGui::Button(fmt::format("X##key{}", key).c_str()))
            {
                auto scan_key  = SDL_GetScancodeFromName(key.c_str());
                auto mouse_btn = input::string_to_mouse(key);
                m_key_map.at(context).unbind_btn(mouse_btn);
                m_key_map.at(context).unbind_key(scan_key);
                action_h.erase(action_h.begin() + i--);
            }
            i++;
        }

        // Does not look good with new line when empty action_h
        if (!action_h.empty())
        {
            ImGui::NewLine();
        }

        ImGui::TableNextCell();
        if (ImGui::Button("Add new Binding"))
        {
            ImGui::OpenPopup(fmt::format("Select Action##{}", context).c_str());
        }

        // Used to open binding popup
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
                    m_bind_index  = m_display_map[context].size();
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
            m_unsaved_changes = true;
            ImGui::Text("Press a Key");
            input::EMouse bind_mouse = input::EMouse::BtnNone;
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
                // Remove what ever old binding was there
                m_key_map.at(context).unbind_key(n_key);

                // The binding it a replacemnt
                if (m_bind_index >= 0 && m_bind_index < m_display_map[context].size())
                {
                    auto old_key_s = m_display_map[context][m_bind_index].first;
                    m_bind_action  = m_display_map[context][m_bind_index].second;

                    // Find if that key has been used before
                    auto&& it = std::find_if(m_display_map[context].begin(),
                                             m_display_map[context].end(),
                                             [n_key](const auto& p) { return p.first == SDL_GetScancodeName(n_key); });

                    // If a binding with the key already exist, Flip the actions
                    if (it != m_display_map[context].end())
                    {
                        m_display_map[context][m_bind_index].first = (*it).first;
                        // The old key get the n_key's old action
                        auto scan_c = SDL_GetScancodeFromName(old_key_s.c_str());
                        if (scan_c != SDL_SCANCODE_UNKNOWN)
                        {
                            m_key_map.at(context).bind_key(scan_c, (*it).second);
                        }
                        else
                        {
                            m_key_map.at(context).bind_btn(input::string_to_mouse(old_key_s), (*it).second);
                        }
                        (*it).first = old_key_s;
                    }
                    else
                    {
                        m_display_map[context][m_bind_index] = std::pair(SDL_GetScancodeName(n_key), m_bind_action);
                    }

                    // bind the action to a new key
                    m_key_map.at(context).bind_key(n_key, m_bind_action);
                }
                // The bindings is a new binding
                else if (m_bind_index == m_display_map[context].size())
                {
                    auto&& it = std::find_if(m_display_map[context].begin(),
                                             m_display_map[context].end(),
                                             [n_key](const auto& p) { return p.first == SDL_GetScancodeName(n_key); });

                    // If a binding with the key already exist, remove it
                    if (it != m_display_map[context].end())
                    {
                        m_display_map[context].erase(it);
                        m_key_map.at(context).unbind_key(n_key);
                    }
                    m_display_map[context].emplace_back(std::pair(SDL_GetScancodeName(n_key), m_bind_action));
                    m_key_map.at(context).bind_key(n_key, m_bind_action);
                }

                m_bind_index  = -1;
                m_bind_action = input::EAction::None;
                ImGui::CloseCurrentPopup();
            }
            else if (bind_mouse != input::EMouse::BtnNone)
            {
                m_key_map.at(context).unbind_btn(bind_mouse);

                // The binding it a replacemnt
                if (m_bind_index >= 0 && m_bind_index < m_display_map[context].size())
                {
                    auto old_key_s = m_display_map[context][m_bind_index].first;
                    m_bind_action  = m_display_map[context][m_bind_index].second;

                    // Find if that key has been used before
                    auto&& it =
                        std::find_if(m_display_map[context].begin(), m_display_map[context].end(), [bind_mouse](const auto& p) {
                            return p.first == input::mouse_to_string(bind_mouse);
                        });

                    // If a binding with the key already exist, Flip the actions
                    if (it != m_display_map[context].end())
                    {
                        m_display_map[context][m_bind_index].first = (*it).first;
                        // The old key get the bind_mouse's old action
                        auto scan_c = SDL_GetScancodeFromName(old_key_s.c_str());
                        if (scan_c != SDL_SCANCODE_UNKNOWN)
                        {
                            m_key_map.at(context).bind_key(scan_c, (*it).second);
                        }
                        else
                        {
                            m_key_map.at(context).bind_btn(input::string_to_mouse(old_key_s), (*it).second);
                        }
                        (*it).first = old_key_s;
                    }
                    else
                    {
                        m_display_map[context][m_bind_index] = std::pair(input::mouse_to_string(bind_mouse), m_bind_action);
                    }

                    // bind the action to a new key
                    m_key_map.at(context).bind_btn(bind_mouse, m_bind_action);
                }
                // The bindings is a new binding
                else if (m_bind_index == m_display_map[context].size())
                {
                    auto&& it =
                        std::find_if(m_display_map[context].begin(), m_display_map[context].end(), [bind_mouse](const auto& p) {
                            return p.first == input::mouse_to_string(bind_mouse);
                        });

                    // If a binding with the key already exist, remove it
                    if (it != m_display_map[context].end())
                    {
                        m_display_map[context].erase(it);
                        m_key_map.at(context).unbind_btn(bind_mouse);
                    }
                    m_display_map[context].emplace_back(std::pair(input::mouse_to_string(bind_mouse), m_bind_action));
                    m_key_map.at(context).bind_btn(bind_mouse, m_bind_action);
                }

                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::EndTable();
        ImGui::NewLine();
    }

    ImGui::End();
}
void PreferenceScene::quit_btn()
{
    auto res = std::get<glm::ivec2>(m_context->preferences->get_resolution().value);
    ImGui::SetNextWindowPos(glm::vec2(res.x / 2, res.y / 1.2f), 0, {.5f, .5f});
    ImGui::SetNextWindowSize({230, 100});
    ImGui::Begin("Option##quit",
                 nullptr,
                 ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
    bool success = true;
    if (ImGui::Button("Save##ps", {100, 50}))
    {
        auto&& input = input::get_input();
        input.set_input_map(m_key_map);
        input.save_binding_to_file();
        m_unsaved_changes = false;
    }

    ImGui::SameLine();
    if (ImGui::Button("Back##ps", {100, 50}))
    {
        if (m_unsaved_changes)
        {
            ImGui::OpenPopup("Unsaved Changes");
        }
        else
        {
            m_context->scene_manager->pop();
        }
    }

    if (ImGui::BeginPopupModal("Unsaved Changes", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::Text("You have unsaved changes\nAre you sure you want quit without saving");
        if (ImGui::Button("Yes##ps"))
        {
            m_context->scene_manager->pop();
        }
        ImGui::SameLine();
        if (ImGui::Button("No##ps"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

} // namespace cs
