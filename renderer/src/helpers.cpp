#include "helpers.hpp"

#include <doctest/doctest.h>

namespace ulf
{
uint32_t rate_physical_device(const vk::PhysicalDevice& device)
{
    REQUIRE(device);
    uint32_t score = 0u;

    /* Rate based on properties */
    const auto properties = device.getProperties();

    /* Device is unsuitable if we do not have at least API 1.1 */
    if (properties.apiVersion < VK_API_VERSION_1_1)
    {
        return score;
    }

    /* Heavily prefer discrete GPU */
    if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
    {
        score += 1000u;
    }
    else
    {
        score += 250u;
    }

    /* Rate based on features */
    const auto features = device.getFeatures();
    if (features.multiDrawIndirect)
    {
        score += 150u;
    }

    if (features.geometryShader && features.tessellationShader)
    {
        score += 250u;
    }

    if (features.samplerAnisotropy)
    {
        score += 150u;
    }

    if (features.shaderStorageBufferArrayDynamicIndexing)
    {
        score += 100u;
    }

    if (features.shaderUniformBufferArrayDynamicIndexing)
    {
        score += 100u;
    }

    REQUIRE(score > 0u);
    return score;
}

}  // namespace ulf
