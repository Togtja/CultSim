#pragma once

#include "scene.h"

namespace cs
{
/**
 * The load scenario scene provides the user with options before running a scenario
 */
class LoadScenarioScene : public IScene
{
private:
    /** Max length of the seed */
    static inline constexpr unsigned s_seed_length = 32;

    /** The seed in a string / human readable format */
    char m_seed[s_seed_length];

public:
    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};
} // namespace cs
