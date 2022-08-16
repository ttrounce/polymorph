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
    create_vma_allocator(*this);
    create_swapchain(*this);
    create_swap_image_views(*this);
    create_render_pass(*this);
    create_swap_framebuffers(*this);
    create_command_pool(*this);
}

void context::cleanup()
{
    vkDestroyCommandPool(device.v_logical, device.v_command_pool, VK_NULL_HANDLE);
    device.v_command_pool = VK_NULL_HANDLE;

    vkDestroyRenderPass(device.v_logical, v_render_pass, VK_NULL_HANDLE);
    v_render_pass = VK_NULL_HANDLE;

    destroy_swapchain(*this);

    vmaDestroyAllocator(this->allocator);
    allocator = VK_NULL_HANDLE;

    vkDestroyDevice(device.v_logical, VK_NULL_HANDLE);
    device.v_logical = VK_NULL_HANDLE;

    vkDestroySurfaceKHR(v_instance, v_surface, VK_NULL_HANDLE);
    v_surface = VK_NULL_HANDLE;

    destroy_debug_messenger(*this);
    v_debug_messenger = VK_NULL_HANDLE;

    vkDestroyInstance(v_instance, VK_NULL_HANDLE);
    v_instance = VK_NULL_HANDLE;
}