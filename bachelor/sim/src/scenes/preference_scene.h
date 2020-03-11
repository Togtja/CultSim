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

    SDL_Scancode m_bind_key      = SDL_SCANCODE_UNKNOWN;
    input::EAction m_bind_action = input::EAction::None;

public:
    PreferenceScene()
    {
        m_key_map = input::get_input().get_input_map();
    }

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

    void key_binding();
};
} // namespace cs
