#pragma once

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
    std::vector<vk::ExtensionProperties> instance_ext{};

    /** Required device extensions */
    std::vector<vk::ExtensionProperties> device_ext{};

    /** Do we prefer using a discrete GPU */
    bool prefer_discrete = true;
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

public:
    RenderContext();
    RenderContext(std::string_view appname);

private:
    vk::Result init_instance(std::string_view appname);
};

}  // namespace ulf
