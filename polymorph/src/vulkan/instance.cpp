#include "polymorph/vulkan/context.h"
#include "polymorph/vulkan/utility.h"
#include "polymorph/vulkan/defines.h"

#include <iostream>

using namespace poly::vk;

// ------------------------- UTILS -------------------------

static VkBool32 debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT           message_severity,
    VkDebugUtilsMessageTypeFlagsEXT                  message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data)
{
    std::cerr << "[VK] " << callback_data->pMessage << std::endl;
    return VK_FALSE;
}

static VkResult create_debug_utils_messenger_ext(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void destroy_debug_utils_messenger_ext(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

static bool check_validation_support(const std::vector<const char*>& requested_layers) {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const char* layer_name : requested_layers) {
        bool layer_found = false;

        for (const auto& layer_props : available_layers) {
            if (strcmp(layer_name, layer_props.layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            return false;
        }
    }
    return true;
}

// ------------------------- INSTANCE, DEBUG, & SURFACE -------------------------

void poly::vk::create_instance(context& context)
{
    VkApplicationInfo app_info{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    app_info.pApplicationName = context.app_name.c_str();
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = VULKAN_ENGINE_NAME;
    app_info.engineVersion = VULKAN_ENGINE_VERSION;
    app_info.apiVersion = VK_API_VERSION_1_2; // Allow use of raytracing, etc.

    VkInstanceCreateInfo create_info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    create_info.pApplicationInfo = &app_info;

    auto required_extensions = get_glfw_extensions();
    create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
    create_info.ppEnabledExtensionNames = required_extensions.data();

#ifdef POLYMORPH_VULKAN_USE_VALIDATION
    ASSERT_VK(check_validation_support(context.requested_layers));

    create_info.enabledLayerCount = static_cast<uint32_t>(context.requested_layers.size());
    create_info.ppEnabledLayerNames = context.requested_layers.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
    debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_create_info.pfnUserCallback = debug_callback;

    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
#endif // POLYMORPH_VULKAN_USE_VALIDATION

    CHECK_VK(vkCreateInstance(&create_info, VK_NULL_HANDLE, &context.v_instance));
}

void poly::vk::create_debug_messenger(context& context)
{
#ifdef POLYMORPH_VULKAN_USE_VALIDATION
    VkDebugUtilsMessengerCreateInfoEXT dbg_create_info{};
    dbg_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    dbg_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    dbg_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    dbg_create_info.pfnUserCallback = debug_callback;

    CHECK_VK(create_debug_utils_messenger_ext(context.v_instance, &dbg_create_info, VK_NULL_HANDLE, &context.v_debug_messenger));
#endif // POLYMORPH_VULKAN_USE_VALIDATION
}

void poly::vk::create_surface(context& context)
{
    CHECK_VK(glfwCreateWindowSurface(context.v_instance, context.glfw_window, VK_NULL_HANDLE, &context.v_surface));
}

void poly::vk::destroy_debug_messenger(context& context)
{
    destroy_debug_utils_messenger_ext(context.v_instance, context.v_debug_messenger, VK_NULL_HANDLE);
}