#pragma once

#include "camera.h"

namespace cs::gfx
{
class RaymarchingRenderer
{
private:
    Camera& m_camera;

    uint32_t m_shader;

public:
    RaymarchingRenderer(Camera& camera);

    ~RaymarchingRenderer() noexcept;

    void display();
};
} // namespace cs::gfx
