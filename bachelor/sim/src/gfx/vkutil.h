#pragma once
#include "glutil.h"

#include <algorithm>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "vma.h"
#include <glm/vec2.hpp>
#include <volk.h>

namespace cs::vk
{
/**
 * Helper struct to be able to pass in a shader with an associated stage
 */
struct ShaderModuleAndStage
{
    VkShaderModule module{VK_NULL_HANDLE};
    VkShaderStageFlagBits stage{VK_SHADER_STAGE_ALL};
};

/**
 * Represents a buffer resource with a memory handle
 */
struct Buffer
{
    VkBuffer buffer{VK_NULL_HANDLE};
    VmaAllocation allocation{VK_NULL_HANDLE};
    void* data = nullptr;
};

struct Swapchain
{
    VkSwapchainKHR swapchain{VK_NULL_HANDLE};
    std::vector<VkImage> images{};
    std::vector<VkImageView> views{};
    std::vector<VkFramebuffer> framebuffers{};
    VkSurfaceFormatKHR format{};
    VkExtent2D size{};
};

uint32_t get_queue_index(VkPhysicalDevice pdev, VkQueueFlags required_flags);

VkPresentModeKHR select_present_mode(VkPresentModeKHR desired, std::vector<VkPresentModeKHR>& avail);

VkSurfaceFormatKHR select_surface_format(const std::vector<VkSurfaceFormatKHR>& avail);

/**
 * Create a new swapchain or resize an existing one
 *
 * @param device
 * @param pdevice
 * @param surface
 * @param surface_caps
 * @param render_pass
 * @param queue_idx
 * @param format
 * @param out Where the new swapchain will be stored, if there is already a valid swapchain here, it will be deleted and resized
 */
void create_swapchain(VkDevice device,
                           VkPhysicalDevice pdevice,
                           VkSurfaceKHR surface,
                           VkSurfaceCapabilitiesKHR surface_caps,
                           VkRenderPass render_pass,
                           uint32_t queue_idx,
                           VkSurfaceFormatKHR format,
                           Swapchain& out);

void destroy_swapchain(VkDevice device, Swapchain& swapchain);

VkRenderPass create_render_pass(VkDevice device, VkFormat format);

VkImageView create_image_view(VkDevice device, VkImage image, VkFormat format);

VkFramebuffer create_framebuffer(VkDevice device, VkRenderPass rp, VkImageView image_view, VkExtent2D extent);

VkSemaphore create_semaphore(VkDevice device);

VkFence create_fence(VkDevice device, VkBool32 start_signalled);

VkCommandPool create_command_pool(VkDevice device, uint32_t queue_index, VkCommandPoolCreateFlags flags);

VkCommandBuffer begin_one_time_cmd_buffer(VkDevice device, VkCommandPool pool);

void end_one_time_cmd_buffer(VkCommandBuffer buffer);

VkImageMemoryBarrier create_image_barrier(VkImage image,
                                          VkAccessFlags srcAccess,
                                          VkAccessFlags dstAccess,
                                          VkImageLayout oldLayout,
                                          VkImageLayout newLayout);

void transition_image(VkCommandBuffer buffer, VkImage image, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage);

VkShaderModule load_shader(VkDevice device, std::string_view rpath);

VkPipelineLayout create_pipeline_layout(VkDevice device, const std::vector<VkDescriptorSetLayout>& descriptor_layouts);

VkPipeline create_gfx_pipeline(VkDevice device,
                               VkPipelineCache pipecache,
                               VkRenderPass render_pass,
                               VkPipelineLayout layout,
                               const std::vector<ShaderModuleAndStage>& shaders);

VmaAllocator create_allocator(VkInstance instance, VkDevice device, VkPhysicalDevice pdevice);

Buffer create_buffer(VmaAllocator allocator, size_t size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage);

void destroy_buffer(VmaAllocator allocator, const Buffer& buffer);

VkDescriptorSetLayout create_descriptor_set_layout(VkDevice device);

VkDescriptorPool create_descriptor_pool(VkDevice device);

} // namespace cs::vk
