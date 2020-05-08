#pragma once

#include "camera.h"
#include "render_data.h"

#include <vector>

namespace cs::gfx
{
/**
 * A simplified spher tracing / ray marching renderer to do the job of 3D visualization within CultSim
 *
 * @note Currently in a proof of concept mode until further notice
 */
class RaymarchingRenderer
{
private:
    /** Reference to the camera */
    Camera& m_camera;

    /** Active shader */
    uint32_t m_shader;

    /** VBO to hold the spheres we are drawing */
    uint32_t m_sphere_buffer;

    /** The CPU-side sphere data before flushing to the VBO */
    std::vector<SphereShape> m_spheres{};

public:
    RaymarchingRenderer(Camera& camera);

    ~RaymarchingRenderer() noexcept;

    /**
     * Clear all spheres from the renderer so we can submit new ones
     */
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
