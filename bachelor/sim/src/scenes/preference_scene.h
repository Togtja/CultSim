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
public:
    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
