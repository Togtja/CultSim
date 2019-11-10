#pragma once

#include "helpers.hpp"

#include <array>
#include <vector>

#include <vulkan/vulkan.hpp>

typedef struct SDL_Window SDL_Window;

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

    /** Window for context */
    SDL_Window* window{nullptr};
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

    /** Context surface, created from the window */
    vk::SurfaceKHR m_surface{};

public:
    /** Constructors */
    RenderContext() = default;
    ULFEYE_NO_COPY(RenderContext);
    ULFEYE_NO_MOVE(RenderContext);
    ~RenderContext() noexcept;

    /**
     * @brief initialize the render context using the provided information
     * @param appname is the internal name of the application, has no effect except it names the app
     * @param settings contain layers and extensions that we want to have enabled
     * @return result of initialization, do NOT use the Context unless this is vk::Result::eSuccess, otherwise handle it
     */
    vk::Result initialize(std::string_view appname, const RenderContextSettings& settings);

    /**
     * @brief returns the logical device to the caller
     */
    vk::Device& device();

    /**
     * @brief returns the graphics queue to the caller
     */
    vk::Queue& gfx_queue();

    /**
     * @brief returns the queue family index of the graphics queue
     */
    uint32_t gfx_idx() const;

    /**
     * @brief returns the compute queue to the caller
     */
    vk::Queue& compute_queue();

    /**
     * @brief returns the queue family index of the compute queue
     */
    uint32_t compute_idx() const;

    /**
     * @brief returns the surface that this rendercontext contains
     */
    vk::SurfaceKHR& surface();

private:
    /**
     * @brief initialize the Vulkan instance
     * @param appname is the internal name of the application to identify the Instance
     * @param layer_names contain a vector of desired validation layers to enable
     * @param ext_names contain a vector of desired instance extensions to enable
     */
    vk::Result init_instance(std::string_view appname, const std::vector<const char*>& layer_names,
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
    vk::Result init_device(const vk::PhysicalDeviceFeatures& features, const std::vector<const char*>& layer_names,
                           const std::vector<const char*>& ext_names);

    /**
     * @brief initialize a surface from the given window
     * @param window is the window to create a surface in
     */
    vk::Result init_surface(SDL_Window* window);
};

}  // namespace ulf
