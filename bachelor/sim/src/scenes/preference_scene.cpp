#include "preference_scene.h"
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
            ImGui::InputText(input::action_to_string(action).c_str(),
                             m_key_buff[context][i].data(),
                             m_key_buff[context][i].size() + 1);

            ImGui::TableNextCell();
            auto action_s = input::action_to_string(action);
            ImGui::Text(action_s.c_str());

            ImGui::TableNextCell();
            if (ImGui::Button(fmt::format("X##{}", i).c_str()))
            {
                action_h.unbind_key(key);
                m_key_buff[context].erase(m_key_buff[context].begin() + i--);
            }
            i++;
        }
        // TODO: Do this for mouse bindings

        ImGui::EndTable();
        if (ImGui::Button("Add new Binding"))
        {
            ImGui::OpenPopup(fmt::format("Select Action{}", context).c_str());
}

        if (ImGui::BeginPopupModal(fmt::format("Select Action{}", context).c_str(),
                                   nullptr,
                                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
        {
            input::EAction action_enum;
            for (int i = static_cast<int>(input::EAction::None) + 1; i < static_cast<int>(input::EAction::Count); i++)
            {
                if (ImGui::Button(input::action_to_string(static_cast<input::EAction>(i)).c_str()))
                {
                    action_enum = static_cast<input::EAction>(i);
                    m_key_buff[context].emplace_back("A");
                    action_h.bind_key(static_cast<SDL_Scancode>(free_codes++), action_enum);
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
        ImGui::NewLine();
    }

    ImGui::End();
}

} // namespace cs
