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
    std::vector<const char*> get_glfw_extensions();

    struct queue_families
    {
        std::optional<uint32_t> graphics;
        std::optional<uint32_t> present;
        std::optional<uint32_t> compute;
        std::optional<uint32_t> transfer;

        bool is_comprehensive()
        {
            return graphics.has_value() && present.has_value() && compute.has_value() && transfer.has_value();
        }
    };

    queue_families get_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);

    struct swapchain_support_details
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    swapchain_support_details get_swapchain_support_details(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

    uint32_t find_memory_type(VkPhysicalDevice device, uint32_t type_filter, VkMemoryPropertyFlags memory_props);
}