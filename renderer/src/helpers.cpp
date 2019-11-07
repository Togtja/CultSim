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

uint32_t get_queue_index(const vk::PhysicalDevice& pdev, vk::QueueFlags required_flags)
{
    /* Get the queues of the device and identify a graphics and present queue */
    const auto queue_info = pdev.getQueueFamilyProperties();

    /* Identify all queue families */
    uint32_t idx = 0u;

    /* Look for dedicated queue that support flag(s) */
    for (const auto& queue : queue_info)
    {
        if ((queue.queueFlags & required_flags) == required_flags && !(queue.queueFlags & ~required_flags))
        {
            return idx;
        }
        ++idx;
    }

    /* Look for generic queue that support flag(s) */
    idx = 0u;
    for (const auto& queue : queue_info)
    {
        if ((queue.queueFlags & required_flags))
        {
            return idx;
        }
        ++idx;
    }

    return idx;
}

/** --- TESTS --- */

TEST_CASE("Filtering Helpers")
{
    /* Avail Extensions */
    const auto ext_1 = VkExtensionProperties{"Hello", 0u};
    const auto ext_2 = VkExtensionProperties{"World", 0u};
    const auto ext_3 = VkExtensionProperties{"Extension", 0u};
    const std::vector<vk::ExtensionProperties> ext_avail{ext_1, ext_2, ext_3};

    /* Avail Layers */
    const auto lay_1 = VkLayerProperties{"Validation", 0u, 0u, ""};
    const auto lay_2 = VkLayerProperties{"Renderdoc", 0u, 0u, ""};
    const auto lay_3 = VkLayerProperties{"Debug", 0u, 0u, ""};
    const std::vector<vk::LayerProperties> layer_avail{lay_1, lay_2, lay_3};

    SUBCASE("Filter Extensions")
    {
        /* Test Cases */
        const auto desired_a = std::vector<const char*>{"Hello", "World"};
        const auto desired_b = std::vector<const char*>{"Hello", "Norway"};
        const auto desired_c = std::vector<const char*>{};

        /* Results */
        const auto chosen_a = filter_desired_to_available_extensions(desired_a, ext_avail);
        const auto chosen_b = filter_desired_to_available_extensions(desired_b, ext_avail);
        const auto chosen_c = filter_desired_to_available_extensions(desired_c, ext_avail);

        /* Asserts */
        REQUIRE(chosen_a.size() == desired_a.size());
        REQUIRE(chosen_b.size() == 1u);
        REQUIRE(chosen_c.empty());
    }

    SUBCASE("Filter Layers")
    {
        /* Test Cases */
        const auto desired_a = std::vector<const char*>{"Validation", "Debug"};
        const auto desired_b = std::vector<const char*>{"Renderdoc", "Google"};
        const auto desired_c = std::vector<const char*>{};

        /* Results */
        const auto chosen_a = filter_desired_to_available_layers(desired_a, layer_avail);
        const auto chosen_b = filter_desired_to_available_layers(desired_b, layer_avail);
        const auto chosen_c = filter_desired_to_available_layers(desired_c, layer_avail);

        /* Asserts */
        REQUIRE(chosen_a.size() == desired_a.size());
        REQUIRE(chosen_b.size() == 1u);
        REQUIRE(chosen_c.empty());
    }
}

}  // namespace ulf
