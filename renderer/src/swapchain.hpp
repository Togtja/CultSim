#pragma once

#include "helpers.hpp"

#include <vector>

#include <vulkan/vulkan.hpp>

namespace ulf
{
/**
 * @brief Contain settings and parameters for a swap chain
 */
struct SwapchainSettings
{
    vk::SwapchainKHR old{};
    vk::Extent2D extent{};
    vk::SurfaceFormatKHR surface_format{};
    vk::ImageUsageFlags image_usage{};
    vk::CompositeAlphaFlagBitsKHR composite_alpha{};
    vk::PresentModeKHR present_mode{};
    vk::SurfaceTransformFlagBitsKHR pre_transform;
    uint32_t image_count{3u};
    uint32_t array_layers{0u};
};

/**
 * Contain the images we are presenting to and the swapchain.
 */
class Swapchain
{
private:
    /** Vulkan device that own the swapchain */
    vk::Device& m_device;

    /** Surface (does not own it, other party will destroy it) */
    vk::SurfaceKHR m_surface{};

    /** The handle to the managed swapchain */
    vk::SwapchainKHR m_swapchain{};

    /** The images of the swapchain */
    std::vector<vk::Image> m_images{};

    /** Contain swapchain settings */
    SwapchainSettings m_settings;

public:
    Swapchain(vk::Device& device, vk::SurfaceKHR surface);
    ULFEYE_NO_COPY(Swapchain);
    ~Swapchain() noexcept;

    /**
     * @brief default initialize the swapchain with the given physical device using sensible settings
     * @param pdev is the physical device to use for getting information about surface formats and color spaces
     */
    void initialize(vk::PhysicalDevice& pdev);

    /**
     * @brief initialize the swapchain with the given physical device and swap chain settings
     * @param pdev is the physical device to use for getting information about surface formats and color spaces
     * @param settings contain the desired settings of the swapchain
     */
    void initialize(vk::PhysicalDevice& pdev, const SwapchainSettings& settings);

private:
};

}  // namespace ulf
