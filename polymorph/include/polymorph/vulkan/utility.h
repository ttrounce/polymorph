#pragma once

// cannot use anything from context.h (circular)

#include "../error.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

#define CHECK_VK(expr)  { if((expr) != VK_SUCCESS) { print_error("Vulkan", #expr, __FILE__, __LINE__); } }
#define ASSERT_VK(expr) { if(!(expr)) { print_error("Vulkan", #expr, __FILE__, __LINE__); } }

namespace poly::vk
{
    std::vector<const char*> get_glfw_extensions();

    struct queue_families
    {
        uint32_t graphics = -1;
        uint32_t present = -1;
        uint32_t compute = -1;
        uint32_t transfer = -1;

        bool is_comprehensive()
        {
            return graphics != -1 && present != -1 && compute != -1 && transfer != -1;
        }
    };

    queue_families get_queue_families(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

    struct swapchain_support_details
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    swapchain_support_details get_swapchain_support_details(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
}