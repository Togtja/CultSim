#pragma once

#include "gfx/render_data.h"
#include "gfx/raymarching_renderer.h"
#include "system.h"

namespace cs::system
{
class Rendering : public ISystem
{
private:
    gfx::SpriteTextureID m_shadow_texture{};

    gfx::Camera m_camera{};

    gfx::RaymarchingRenderer m_3d_renderer{m_camera};

public:
    Rendering(SystemContext context);

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
