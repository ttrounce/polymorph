#include "polymorph/vulkan/context.h"
#include "polymorph/vulkan/utility.h"
#include "polymorph/vulkan/defines.h"

#include <assert.h>
#include <iostream>

using namespace poly::vk;

void context::init(const std::string& app_name, const std::vector<const char*>& requested_layers, std::vector<const char*>& requested_device_extensions, GLFWwindow* glfw_window)
{
    this->app_name = app_name;
    this->requested_layers = requested_layers;
    this->requested_device_extensions = requested_device_extensions;
    this->glfw_window = glfw_window;

    create_instance(*this);
    create_debug_messenger(*this);
    create_surface(*this);
    create_physical_device(*this);
    create_logical_device(*this);
    create_swapchain(*this);
    create_swap_image_views(*this);
    create_render_pass(*this);
    create_swap_framebuffers(*this);
    create_command_pool(*this);
    create_graphics_command_buffer(*this);
}

void context::cleanup()
{
    vkDestroyCommandPool(device.v_logical, device.v_command_pool, VK_NULL_HANDLE);
    vkDestroyRenderPass(device.v_logical, v_render_pass, VK_NULL_HANDLE);

    for (auto& framebuf : swapchain.framebuffers)
    {
        destroy_framebuffer(*this, framebuf);
    }
    for (auto view : swapchain.image_views)
    {
        vkDestroyImageView(device.v_logical, view, nullptr);
    }

    vkDestroySwapchainKHR(device.v_logical, swapchain.v_swapchain, VK_NULL_HANDLE);
    vkDestroyDevice(device.v_logical, VK_NULL_HANDLE);
    vkDestroySurfaceKHR(v_instance, v_surface, VK_NULL_HANDLE);
    destroy_debug_messenger(*this);
    vkDestroyInstance(v_instance, VK_NULL_HANDLE);
}