#pragma once

#include "camera.h"

namespace cs::gfx
{
class RaymarchingRenderer
{
private:
    /** Reference to the camera */
    Camera& m_camera;

    /** Active shader */
    uint32_t m_shader;

public:
    RaymarchingRenderer(Camera& camera);

    ~RaymarchingRenderer() noexcept;

    /**
     * Make the renderer display it's data to the screen
     */
    void display();

    /**
     * Reload the renderer to it's initial state
     */
    void reload();

private:
    void init();

    void deinit();
};
} // namespace cs::gfx
