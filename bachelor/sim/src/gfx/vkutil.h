#pragma once
#include "glutil.h"

#include <algorithm>
#include <unordered_map>

#include <volk.h>

namespace cs::vk
{
VkPresentModeKHR select_present_mode(VkPresentModeKHR desired, std::vector<VkPresentModeKHR>& avail);

VkSurfaceFormatKHR select_surface_format(const std::vector<VkSurfaceFormatKHR>& avail);

VkRenderPass create_render_pass(VkDevice device);

VkImageView create_image_view(VkDevice device, VkImage image);

VkFramebuffer create_framebuffer(VkDevice device, VkRenderPass rp, VkImageView image_view, VkExtent2D extent);
} // namespace cs::vk
