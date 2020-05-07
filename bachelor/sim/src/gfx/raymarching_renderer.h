#pragma once

#include "camera.h"
#include "render_data.h"

#include <vector>

namespace cs::gfx
{
/** TODO: Documentation */
class RaymarchingRenderer
{
private:
    /** Reference to the camera */
    Camera& m_camera;

    /** Active shader */
    uint32_t m_shader;

    uint32_t m_sphere_buffer;

    std::vector<SphereShape> m_spheres{};

public:
    RaymarchingRenderer(Camera& camera);

    ~RaymarchingRenderer() noexcept;

    void clear();

    /**
     * Make the renderer display it's data to the screen
     */
    void display();

    /**
     * Reload the renderer to it's initial state
     */
    void reload();

    /**
     * Draw a sphere
     *
     * @param pos Position to draw it
     * @param r Radius of sphere
     */
    void draw_sphere(const glm::vec3& pos, float r);

private:
    void init();

    void deinit();
};
} // namespace cs::gfx
