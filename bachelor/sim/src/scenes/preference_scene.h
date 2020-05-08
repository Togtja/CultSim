#pragma once

#include "input/input_handler.h"
#include "scene.h"

#include <string>
#include <utility>
#include <vector>

namespace cs
{
/**
 * Presents the preferences available to the user so they may change them, including key bindings
 */
class PreferenceScene : public IScene
{
private:
    /** Key bindings map */
    robin_hood::unordered_map<input::EKeyContext, input::detail::ActionHandler> m_key_map{};

    /** Brief To keep things looking consistent in the main menu there is a display map */
    robin_hood::unordered_map<input::EKeyContext, std::vector<std::pair<std::string, input::EAction>>> m_display_map{};

    /** Index into display vector for current key */
    int m_bind_index{-1};

    /** The current action to bind (if active) */
    input::EAction m_bind_action{input::EAction::None};

    /** Are there any unsaved preference changes */
    bool m_unsaved_changes{false};

public:
    PreferenceScene();

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

private:
    /** Show the UI for key bindings */
    void show_keybinding_ui();

    /** Quit back to the previous menu */
    void go_back();

    /**
     * Displays the avaliable action you can bind a key to  given a context
     *
     * @param context The context ro extract your action options
     */
    void display_select_action_ui(input::EKeyContext context);

    /**
     * Dispays the "press a key" screen, where you press a key and the binding will be change to that key
     *
     * @param context The context to which this new key to binding mapping will be
     */
    void display_change_bindings_ui(input::EKeyContext context);
};
} // namespace cs
