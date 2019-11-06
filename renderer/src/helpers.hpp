#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>

#define ULFEYE_NO_COPY(classname)         \
    classname(const classname&) = delete; \
    classname& operator=(const classname&) = delete

#define ULFEYE_NO_MOVE(classname)    \
    classname(classname&&) = delete; \
    classname& operator=(classname&&) = delete

namespace ulf
{
/**
 * @brief Rates a physical device based on it's "qualities" as a device to use in the host application. Will bias and prefer
 * discrete GPU's if available.
 */
uint32_t rate_physical_device(const vk::PhysicalDevice& device);

/**
 * @brief given a list of desired layer names for either the instance or device, filter based on the avilable ones
 * @param desired_layers are the layers we want to have enabled
 * @param avail_layers contain the layers we can choose from
 * @return a vector of the layers we want and are available
 */
std::vector<const char*> filter_desired_to_available_layers(const std::vector<const char*>& desired_layers,
                                                            const std::vector<vk::LayerProperties>& avail_layers);

/**
 * @brief given a list of desired extensions names for either the instance or device, filter based on the avilable ones
 * @param desired_extensions are the extensions we want to have enabled
 * @param avail_extensions contain the extensions we can choose from
 * @return a vector of the layers we want and are available
 */
std::vector<const char*> filter_desired_to_available_extensions(const std::vector<const char*>& desired_extensions,
                                                                const std::vector<vk::ExtensionProperties>& aviail_extensions);

/**
 * @brief get_queue_index gets the index of the first queue that supports all of the required_flags. It will prefer any queue that
 * supports ONLY the given flags, but fall back on a queue family that supports at least the given flags otherwise.
 * @param pdev is the physical device to query
 * @param required_flags are the flags we need the queue to support
 * @return index of the queue that was found
 */
uint32_t get_queue_index(const vk::PhysicalDevice& pdev, vk::QueueFlags required_flags);

}  // namespace ulf
