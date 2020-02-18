#pragma once

#include "camera.h"
#include "render_data.h"
#include "vkutil.h"

#include <string_view>
#include <vector>

#include "vma.h"
#include <volk.h>

namespace cs::gfx
{
/**
 * Vulkan like helper struct to create a sprite renderer
 */
struct SpriteRendererCreateInfo
{
    VkSwapchainKHR swapchain{VK_NULL_HANDLE};
    const std::vector<VkImage>& sc_images{};
    const std::vector<VkImageView>& sc_image_views{};
    VkFormat sc_format{VK_FORMAT_UNDEFINED};
    uint32_t gfx_queue_idx{};
    VkQueue gfx_queue{VK_NULL_HANDLE};
    VmaAllocator allocator{VK_NULL_HANDLE};
};

class SpriteRenderer
{
private:
    Camera& m_camera;

    VkDevice m_device{VK_NULL_HANDLE};

    VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};

    std::vector<VkImage> m_sc_images;

    VkQueue m_gfx_queue{VK_NULL_HANDLE};

    VmaAllocator m_allocator{VK_NULL_HANDLE};

    VkRenderPass m_renderpass{VK_NULL_HANDLE};

    std::vector<VkFramebuffer> m_framebuffers{};

    VkPipelineLayout m_pipeline_layout{VK_NULL_HANDLE};

    VkPipeline m_pipeline{VK_NULL_HANDLE};

    VkCommandPool m_cmd_pool{VK_NULL_HANDLE};

    VkSemaphore m_aq_sem{VK_NULL_HANDLE};

    VkSemaphore m_rel_sem{VK_NULL_HANDLE};

    vk::Buffer m_instance_buffer{};

    /** Pointer to GPU memory where the instance variables are */
    SpriteInstanceVertex* m_instance_data{};

    /** Texture handles for color maps */
    std::vector<uint32_t> m_color_texture_handles{};

    /** Texture handles for normal maps */
    std::vector<uint32_t> m_normal_texture_handles{};

    /** Current number of sprites about to be drawn */
    uint32_t m_nsprites = 0u;

    /** Next available texture ID for sprites */
    SpriteTextureID m_next_texture_id{};

public:
    explicit SpriteRenderer(Camera& camera);

    friend class Renderer;

    /**
     * Add a new sprite to the draw list using the given position color and texture
     *
     * @param pos The position to draw the sprite at in world coordinates
     * @param color The color tint to apply to the sprite texture
     * @param tex The texture ID of the texture that you want to draw
     */
    void draw(glm::vec3 pos, glm::vec3 color, SpriteTextureID tex);

    /**
     * Display submits the draw list for this frame to the GPU and renders the scene
     */
    void display();

    /**
     * Get the texture id of a given texture
     *
     * @param rpath Relative path to the texture file you want the ID of
     * @return An ID to the given texture
     */
    SpriteTextureID get_texture(std::string_view rpath);

private:
    void init(const SpriteRendererCreateInfo& create_info);

    void init_pipeline();

    void deinit();

    bool increment_next_texture_id();

    void init_texture_slots();
};

} // namespace cs::gfx
