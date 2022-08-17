#pragma once

// cannot use anything from context.h (circular)

#include "../error.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <optional>

#define CHECK_VK(expr)  { if((expr) != VK_SUCCESS) { print_error("Vulkan", #expr, __FILE__, __LINE__); } }
#define ASSERT_VK(expr) { if(!(expr)) { print_error("Vulkan", #expr, __FILE__, __LINE__); } }
#define THROW_VK(error_msg) { printf("Vulkan error @ (file: %s, line: %d): %s\n", __FILE__, __LINE__, error_msg); throw std::runtime_error(error_msg); }

namespace poly::vk
{
    /*! @brief Retrieves vulkan extensions requested by GLFW.
    *   @related context
    *   @sa @ref context
    *   @return A list of vulkan extensions requested by GLFW.
    *   @since Indev
    */
    std::vector<const char*> get_glfw_extensions();

    struct queue_families
    {
        std::optional<uint32_t> graphics;
        std::optional<uint32_t> present;
        std::optional<uint32_t> compute;
        std::optional<uint32_t> transfer;

        /*! @brief Returns true if all capabilities are found within the retrieved queue families.
        *   @memberof queue_families
        *   @sa @ref device
        *   @return True if all capabilities are available, false otherwise.
        *   @since Indev
        */
        bool is_comprehensive()
        {
            return graphics.has_value() && present.has_value() && compute.has_value() && transfer.has_value();
        }
    };

    /*! @brief Finds suitable queue families for a given physical device and surface.
    *   @memberof device
    *   @sa @ref device
    *   @sa @ref queue_families
    *   @param[in] device A VkPhysicalDevice handle.
    *   @param[in] surface A VkSurfaceKHR handle.
    *   @return A struct detailing suitable queue family indices.
    *   @since Indev
    */
    queue_families get_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);

    struct swapchain_support_details
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    /*! @brief Retrieves information on the formats, capabilities and present modes supported by a given swapchain.
    *   @memberof swapchain
    *   @sa @ref swapchain
    *   @sa @ref swapchain_support_details
    *   @param[in] device
    *   @param[in] surface
    *   @return A struct containing support details of the given swapchain.
    *   @todo Move into a more appropriate class.
    *   @since Indev
    */
    swapchain_support_details get_swapchain_support_details(VkPhysicalDevice device, VkSurfaceKHR surface);

    /*! @brief Finds the memory type given the physical device and memory properties.
    *   @related buffer
    *   @sa @ref buffer
    *   @param[in] device
    *   @param[in] type_filter
    *   @param[in] memory_props
    *   @since Indev
    */
    uint32_t find_memory_type(VkPhysicalDevice device, uint32_t type_filter, VkMemoryPropertyFlags memory_props);
}