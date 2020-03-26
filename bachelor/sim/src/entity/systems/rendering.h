#pragma once

#include "gfx/render_data.h"
#include "system.h"

namespace cs::system
{
enum class EVisualizationMode : int
{
    None,
    Vision,
    Needs,
    Health
};

class Rendering : public ISystem
{
private:
    EVisualizationMode m_visualization_mode{EVisualizationMode::None};

    gfx::SpriteTextureID m_shadow_texture{};

public:
    Rendering(SystemContext context);

    void update(float dt) override;

    ISystem* clone() override;
};
} // namespace cs::system
