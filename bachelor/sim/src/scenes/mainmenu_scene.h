#pragma once

#include "scene.h"

#include <glm/vec2.hpp>

namespace cs
{
/** Fwd declaration */
struct Preference;

/**
 * Scene that acts as the main entry point to the program and let users start simulations or change preferences
 */
class MainMenuScene : public IScene
{
private:
    /** Keep track of menu size relative to screen */
    glm::ivec2 m_main_menu_size{};

    /** Keep track of window size */
    glm::ivec2 m_window_size{};

public:
    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

private:
    /** Handle event for resolution changes to keep menu centered */
    void handle_size_changed(const Preference& before, const Preference& after);
};
} // namespace cs
