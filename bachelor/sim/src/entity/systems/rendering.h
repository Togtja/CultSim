#pragma once

#include "gfx/render_data.h"
#include "system.h"

namespace cs::system
{
/** TODO: Documentation */
class Rendering : public ISystem
{
private:
    gfx::SpriteTextureID m_shadow_texture{};

    gfx::Camera m_camera{};

public:
    using ISystem::ISystem;

    void initialize() override;

    void deinitialize() override;

    void update(float dt) override;

    ISystem* clone() override;

    void update_imgui() override;
};

} // namespace cs::system
