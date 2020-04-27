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

public:
    Rendering(SystemContext context);

    void initialize() override;

    void deinitialize() override;

    void update(float dt) override;

    ISystem* clone() override;

    void update_imgui() override;
};

} // namespace cs::system
