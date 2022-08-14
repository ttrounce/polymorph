#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "utility.h"

namespace poly::vk
{
    struct swapchain
    {
        VkSwapchainKHR       v_swapchain;

        VkSurfaceFormatKHR   v_surface_format;
        VkPresentModeKHR     v_present_mode;
        VkExtent2D           v_extent;

        std::vector<VkImage> images;
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

    void create_instance        (context&); // instance.cpp
    void create_debug_messenger (context&); // instance.cpp
    void create_surface         (context&); // instance.cpp
    void create_physical_device (context&); // device.cpp
    void create_logical_device  (context&); // device.cpp
    void create_swapchain       (context&); // swapchain.cpp

    void destroy_debug_messenger(context&); // instance.cpp
}