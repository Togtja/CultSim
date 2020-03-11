#pragma once

#include "gfx/renderer.h"
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

public:
    using ISystem::ISystem;

    void update(float dt) override;
};
} // namespace cs::system
