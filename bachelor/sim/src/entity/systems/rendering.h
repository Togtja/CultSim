#pragma once

#include "system.h"
#include "gfx/renderer.h"

namespace cs::system
{
class Rendering : public ISystem
{
public:
    using ISystem::ISystem;

    void update(float dt) override;
};
} // namespace cs::system
