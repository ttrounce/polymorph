#include <polymorph/vulkan/utility.h>
#include <polymorph/vulkan/defines.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

std::vector<const char*> poly::vk::get_glfw_extensions()
{
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

#ifdef POLYMORPH_VULKAN_USE_VALIDATION
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return extensions;
}

poly::vk::queue_families poly::vk::get_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    poly::vk::queue_families qf {};

    uint32_t qf_count {};
    vkGetPhysicalDeviceQueueFamilyProperties(device, &qf_count, VK_NULL_HANDLE);

    ASSERT_VK(qf_count > 0);

    std::vector<VkQueueFamilyProperties> qf_properties(qf_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &qf_count, qf_properties.data());

    for(uint32_t i = 0; i < qf_properties.size(); i++)
    {
        if(qf_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            qf.graphics = i;
        }

        if(qf_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            qf.compute = i;
        }

        if(qf_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            qf.transfer = i;
        }

        VkBool32 present_supported {};
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_supported);
        if(present_supported)
        {
            qf.present = i;
        }

        if(qf.is_comprehensive()) // We break if we have satisfied all required queue families.
        {
            break;
        }
    }
    return qf;
}

poly::vk::swapchain_support_details poly::vk::get_swapchain_support_details(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    poly::vk::swapchain_support_details ssd {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &ssd.capabilities);
    
    uint32_t format_count {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, VK_NULL_HANDLE);
    if(format_count > 0)
    {
        ssd.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, ssd.formats.data());
    }

    uint32_t present_mode_count {};
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);

    if (present_mode_count > 0)
    {
        ssd.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, ssd.present_modes.data());
    }

    return ssd;
}

uint32_t poly::vk::find_memory_type(VkPhysicalDevice device, uint32_t type_filter, VkMemoryPropertyFlags memory_props)
{
    VkPhysicalDeviceMemoryProperties mem_props{};
    vkGetPhysicalDeviceMemoryProperties(device, &mem_props);

    for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++) {
        if (type_filter & (1 << i) && (mem_props.memoryTypes[i].propertyFlags & memory_props) == memory_props) {
            return i;
        }
    }
    THROW_VK("failed to find memory type");
}