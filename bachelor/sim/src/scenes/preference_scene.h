#pragma once

#include "input/input_handler.h"
#include "scene.h"

#include <string>
#include <vector>

namespace cs
{
class PreferenceScene : public IScene
{
private:
    robin_hood::unordered_map<input::EKeyContext, input::detail::ActionHandler> m_key_map;
    robin_hood::unordered_map<input::EKeyContext, std::vector<std::string>> m_key_buff;
    int free_codes = 287;

public:
    PreferenceScene()
    {
        m_key_map = input::get_input().get_input_map();
        for (auto&& [context, action_h] : m_key_map)
        {
            for (auto&& [key, _] : action_h.get_key_binding())
            {
                m_key_buff[context].push_back({SDL_GetScancodeName(key)});
            }
        }
    }

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

    void key_binding();
};
} // namespace cs
