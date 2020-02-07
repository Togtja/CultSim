#pragma once

#include "system.h"
#include "gfx/renderer.h"

namespace cs::system
{
class Rendering : public ISystem
{
private:
    gfx::Renderer& m_renderer;

public:

    Rendering(entt::registry& registry) : ISystem(registry), m_renderer{gfx::get_renderer()}
    {}

    void update(float dt) override;
};
} // namespace cs::system
