#pragma once

#include "camera.h"
#include "debug_renderer.h"
#include "sprite_renderer.h"
#include "window.h"

#include <volk.h>

namespace cs::gfx
{
class Renderer
{
private:
    Camera m_camera{};

    DebugRenderer m_debug_renderer;

    SpriteRenderer m_sprite_renderer;

    VkInstance m_instance{VK_NULL_HANDLE};

    VkPhysicalDevice m_pdevice{VK_NULL_HANDLE};

    VkDevice m_device{VK_NULL_HANDLE};

    uint32_t m_gfx_queue_idx = 0u;

    VkQueue m_gfx_queue{VK_NULL_HANDLE};

    VkSurfaceKHR m_surface{VK_NULL_HANDLE};

    VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};

    std::vector<VkImage> m_swapchain_images{};

    std::vector<VkImageView> m_swapchain_views{};

    VkSurfaceFormatKHR m_format{};

public:
    friend Renderer& get_renderer();

    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&)      = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;
    ~Renderer() noexcept;

    /**
     * Get a handle to the debug renderer
     *
     * @return
     */
    DebugRenderer& debug();
    [[nodiscard]] const DebugRenderer& debug() const;

    /**
     * Get a handle to the sprite renderer
     *
     * @return
     */
    SpriteRenderer& sprite();
    [[nodiscard]] const SpriteRenderer& sprite() const;

    void set_camera_position(glm::vec3 pos);

    void move_camera(glm::vec3 delta);

    void set_camera_bounds(glm::vec2 bounds);

    void init(const Window& window);

private:
    Renderer();

    void create_instance(const Window& window);

    VkPhysicalDevice pick_physical_device(const std::vector<VkPhysicalDevice>& available);

    void create_device();

    void create_swapchain(const Window& window);
};

/**
 * Get access to the Renderer instance
 *
 * @return Reference to the global renderer
 */
Renderer& get_renderer();

} // namespace cs::gfx
