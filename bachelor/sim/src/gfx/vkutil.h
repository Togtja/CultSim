#pragma once
#include "glutil.h"

#include <algorithm>
#include <string_view>
#include <unordered_map>

#include <volk.h>

namespace cs::vk
{
uint32_t get_queue_index(VkPhysicalDevice pdev, VkQueueFlags required_flags);

VkPresentModeKHR select_present_mode(VkPresentModeKHR desired, std::vector<VkPresentModeKHR>& avail);

VkSurfaceFormatKHR select_surface_format(const std::vector<VkSurfaceFormatKHR>& avail);

VkRenderPass create_render_pass(VkDevice device, VkFormat format);

VkImageView create_image_view(VkDevice device, VkImage image, VkFormat format);

VkFramebuffer create_framebuffer(VkDevice device, VkRenderPass rp, VkImageView image_view, VkExtent2D extent);

VkSemaphore create_semaphore(VkDevice device);

VkCommandPool create_command_pool(VkDevice device, uint32_t queue_index, VkCommandPoolCreateFlags flags);

VkCommandBuffer begin_one_time_cmd_buffer(VkDevice device, VkCommandPool pool);

void end_one_time_cmd_buffer(VkCommandBuffer buffer);

VkShaderModule load_shader(VkDevice device, std::string_view rpath);
} // namespace cs::vk
