#pragma once

#include "scene.h"

namespace cs
{
/** TODO: Documentation */
class LoadScenarioScene : public IScene
{
private:
    static inline constexpr unsigned s_seed_length = 32;

    char m_seed[s_seed_length];

public:
    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};
} // namespace cs
