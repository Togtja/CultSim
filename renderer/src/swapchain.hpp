#pragma once

#include "helpers.hpp"

#include <vector>

#include <vulkan/vulkan.hpp>

namespace ulf
{
class Swapchain
{
private:
    vk::Device& m_device;

    vk::SurfaceKHR m_surface{};

    vk::SwapchainKHR m_swapchain{};

    std::vector<vk::Image> m_images{};

    /** Present Mode Priorities - Static for Class */
    inline static auto present_mode_priorities =
        std::array{vk::PresentModeKHR::eFifo, vk::PresentModeKHR::eMailbox, vk::PresentModeKHR::eImmediate};

public:
    Swapchain(vk::Device& device);
    ULFEYE_NO_COPY(Swapchain);
    ~Swapchain() noexcept;
};

}  // namespace ulf
