#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "utility.h"

namespace poly::vk
{
    struct image
    {
        VkImage v_image;
        VkImageView v_view;
        VkDeviceMemory v_memory;
    };

    struct swapchain
    {
        VkSwapchainKHR       v_swapchain;

        VkSurfaceFormatKHR   v_surface_format;
        VkPresentModeKHR     v_present_mode;
        VkExtent2D           v_extent;

        std::vector<VkImage> images;
        std::vector<VkImageView> image_views;
    };

    struct device
    {
        VkDevice         v_logical;
        VkPhysicalDevice v_physical;

        uint32_t         graphics_queue_index;
        uint32_t         present_queue_index;
        uint32_t         compute_queue_index;

        VkQueue          v_graphics_queue;
        VkQueue          v_present_queue;
        VkQueue          v_compute_queue;

        swapchain_support_details swapchain_details;
    };

    struct context
    {
        VkInstance               v_instance;
        VkDebugUtilsMessengerEXT v_debug_messenger;
        VkSurfaceKHR             v_surface;

        device                   device {};
        swapchain                swapchain{};

        void init(const std::string& app_name, const std::vector<const char*>& requested_layers, std::vector<const char*>& requested_device_extensions, GLFWwindow* glfw_window);
        void cleanup();

        std::string              app_name;
        GLFWwindow*              glfw_window;
        std::vector<const char*> requested_layers;
        std::vector<const char*> requested_device_extensions;
    };
                                                                                 
    // Key:
    // - core    => core to the context; called on init.
    // - generic => generic function; called multiple times.                       // | TYPE    | DEFINED AT    |
                                                                                   // |---------|---------------|
    void create_instance         (context&);                                       // | core    | instance.cpp  |
    void create_debug_messenger  (context&);                                       // | core    | instance.cpp  |
    void create_surface          (context&);                                       // | core    | instance.cpp  |
    void create_physical_device  (context&);                                       // | core    | device.cpp    |
    void create_logical_device   (context&);                                       // | core    | device.cpp    |
    void create_swapchain        (context&);                                       // | core    | swapchain.cpp |
    void create_swap_image_views (context&);                                       // | core    | swapchain.cpp |
    void create_image_view       (context&, image&, VkFormat, VkImageAspectFlags); // | generic | image.cpp     |
                                                                                   // |---------|---------------|                            
    void destroy_debug_messenger (context&);                                       // | core    | instance.cpp  |
    void destroy_image           (context&, image&);                               // | generic | image.cpp     |
}