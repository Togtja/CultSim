#pragma once

#include "gfx/camera.h"
#include "gfx/render_data.h"
#include "system.h"

namespace cs::system
{
/**
 * System for calling the renderer with all entities that have drawable components
 */
class Rendering : public ISystem
{
private:
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
