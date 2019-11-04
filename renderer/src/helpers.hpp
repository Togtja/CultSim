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

template<typename T, template<typename> typename Container>
std::ostream& print_container(std::ostream& os, const Container<T>& container, char sep = '\n')
{
    for (const auto& elem : container)
    {
        os << elem << sep;
    }
    return os;
}

}  // namespace ulf
