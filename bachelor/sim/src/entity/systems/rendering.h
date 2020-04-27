#pragma once

#include "gfx/render_data.h"
#include "gfx/raymarching_renderer.h"
#include "system.h"

#include <memory>

namespace cs::system
{
class Rendering : public ISystem
{
private:
    gfx::SpriteTextureID m_shadow_texture{};

    gfx::Camera m_camera{};

    std::unique_ptr<gfx::RaymarchingRenderer> m_3d_renderer{new gfx::RaymarchingRenderer(m_camera)};

public:
    Rendering(SystemContext context);

    void initialize() override;

    void deinitialize() override;

    void update(float dt) override;

    ISystem* clone() override;

    void update_imgui() override;
};

} // namespace cs::system
