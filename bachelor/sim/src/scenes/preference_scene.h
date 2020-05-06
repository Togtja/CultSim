#pragma once

#include "input/input_handler.h"
#include "scene.h"

#include <string>
#include <utility>
#include <vector>

namespace cs
{
/** TODO: Documentation */
class PreferenceScene : public IScene
{
private:
    robin_hood::unordered_map<input::EKeyContext, input::detail::ActionHandler> m_key_map{};

    /** Brief To keep things looking consistent in the main menu there is a display map */
    robin_hood::unordered_map<input::EKeyContext, std::vector<std::pair<std::string, input::EAction>>> m_display_map{};

    int m_bind_index{-1};
    input::EAction m_bind_action{input::EAction::None};

    bool m_unsaved_changes{false};

public:
    PreferenceScene();

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

    void show_keybinding_ui();

    void quit_btn();
};
} // namespace cs
