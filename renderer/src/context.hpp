#pragma once

#include "helpers.hpp"

#include <array>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace ulf
{
/**
 * @struct RenderContextSettings
 * @brief Contains settings for creating a RenderContext
 * @author Carl Findahl
 * @date 03.11.2019
 */
struct RenderContextSettings
{
    /** Required instance extensions */
    std::vector<const char*> instance_ext{};

    /** Required instance extensions */
    std::vector<const char*> instance_layers{};

    /** Required device extensions */
    std::vector<const char*> device_ext{};

    /** Required device extensions */
    std::vector<const char*> device_layers{};

    /** Required device features */
    vk::PhysicalDeviceFeatures device_features{};
};

/**
 * @class RenderContext
 * @brief Contains the Vulkan Instance, and chosen Phyiscal + Logical Device. Bare minimum for having an application with Vulkan.
 * @author Carl Findahl
 * @date 03.11.2019
 */
class RenderContext
{
private:
    /** Vulkan Application Instance */
    vk::Instance m_instance{};

    /** Phyiscal Device (GPU) we are using */
    vk::PhysicalDevice m_pdevice{};

    /** The Logical Device that we operate on */
    vk::Device m_device{};

    /** Queue for graphics, transfer and present operations */
    vk::Queue m_gfx_queue{};

    /** Queue for compute shaders and operations */
    vk::Queue m_compute_queue{};

public:
    /** Constructors */
    ULFEYE_NO_COPY(RenderContext);
    ULFEYE_NO_MOVE(RenderContext);
    ~RenderContext() noexcept;
    explicit RenderContext(const RenderContextSettings& settings = {});
    explicit RenderContext(std::string_view appname, const RenderContextSettings& settings);

private:
    /**
     * @brief initialize the Vulkan instance
     * @param appname is the internal name of the application to identify the Instance
     * @param layer_names contain a vector of desired validation layers to enable
     * @param ext_names contain a vector of desired instance extensions to enable
     */
    void init_instance(std::string_view appname, const std::vector<const char*>& layer_names,
                       const std::vector<const char*>& ext_names);

    /**
     * @brief choose_physical_device selects the best physical device for this application based on the available ones provided
     * @param devices is a vector of the available devices to choose from
     * @return The physical device that is the most suitable
     */
    vk::PhysicalDevice choose_physical_device(const std::vector<vk::PhysicalDevice>& devices);

    /**
     * @brief initialize a logical device
     * @param features are desired features of the device
     * @param layer_names contain a vector of desired validation layers to enable
     * @param ext_names contain a vector of desired instance extensions to enable
     */
    void init_device(const vk::PhysicalDeviceFeatures& features, const std::vector<const char*>& layer_names,
                     const std::vector<const char*>& ext_names);
};

}  // namespace ulf
