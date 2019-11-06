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

std::vector<const char*> filter_desired_to_available_layers(const std::vector<const char*>& desired,
                                                            const std::vector<vk::LayerProperties>& available)
{
    /* Store the avail and desiredlayers to use here */
    std::vector<const char*> out = {};

    /* Look for layers */
    std::copy_if(desired.cbegin(), desired.cend(), std::back_inserter(out), [&available](const char* elem) {
        return std::any_of(available.cbegin(), available.cend(),
                           [&elem](const auto& layer) { return !strcmp(layer.layerName, elem); });
    });

    return out;
}

std::vector<const char*> filter_desired_to_available_extensions(const std::vector<const char*>& desired,
                                                                const std::vector<vk::ExtensionProperties>& aviailable)
{
    /* Store the avail and desired extensions to use here */
    std::vector<const char*> out = {};

    /* Look for extensions */
    std::copy_if(desired.cbegin(), desired.cend(), std::back_inserter(out), [&aviailable](const char* elem) {
        return std::any_of(aviailable.cbegin(), aviailable.cend(),
                           [&elem](const auto& ext) { return !strcmp(ext.extensionName, elem); });
    });

    return out;
}

}  // namespace ulf
