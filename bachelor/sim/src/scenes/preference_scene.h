#pragma once

#include "input/input_handler.h"
#include "scene.h"

#include <string>
#include <utility>
#include <vector>

namespace cs
{
class PreferenceScene : public IScene
{
private:
    robin_hood::unordered_map<input::EKeyContext, input::detail::ActionHandler> m_key_map;
    // To keep things looking consistent in the main menu ther is a display map
    robin_hood::unordered_map<input::EKeyContext, std::vector<std::pair<std::string, input::EAction>>> m_display_map;

    int m_bind_index             = -1;
    input::EAction m_bind_action = input::EAction::None;

    bool m_unsaved_changes = false;

public:
    PreferenceScene()
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

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

    void key_binding();

    void quit_btn();
};
} // namespace cs
