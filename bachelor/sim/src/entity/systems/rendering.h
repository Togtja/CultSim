#pragma once

#include "gfx/render_data.h"
#include "system.h"

namespace cs::system
{
class Rendering : public ISystem
{
private:
    gfx::SpriteTextureID m_shadow_texture{};

public:
    Rendering(SystemContext context);

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
