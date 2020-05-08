#pragma once

#include "scene.h"

namespace cs
{
/** Scene that let users take organizational actions such as quitting Kduring a simulation */
class PauseMenuScene : public IScene
{
public:
    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};
} // namespace cs
