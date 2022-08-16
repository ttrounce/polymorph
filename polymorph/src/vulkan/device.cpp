#include "polymorph/vulkan/context.h"
#include "polymorph/vulkan/utility.h"
#include "polymorph/vulkan/defines.h"

#include <set>

using namespace poly::vk;

// ------------------------- UTILS -------------------------

static bool check_device_extension_support(context& context, const VkPhysicalDevice& device) {
    uint32_t extension_count = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(context.requested_device_extensions.begin(), context.requested_device_extensions.end());

    for (const auto& extension : available_extensions) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

static bool is_device_suitable(context& context, const VkPhysicalDevice& device, const swapchain_support_details& details)
{
    queue_families qf = get_queue_families(device, context.v_surface);
    bool extensions_supported = check_device_extension_support(context, device);
    bool swapchain_supported = false;

    VkPhysicalDeviceProperties properties {};
    vkGetPhysicalDeviceProperties(device, &properties);

    if(extensions_supported)
    {
        swapchain_supported = !details.formats.empty() && !details.present_modes.empty();
    }
    return qf.is_comprehensive() && extensions_supported && swapchain_supported && properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

// ------------------------- DEVICE -------------------------

void poly::vk::create_physical_device(context& context)
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(context.v_instance, &device_count, VK_NULL_HANDLE);
    ASSERT_VK(device_count > 0);

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(context.v_instance, &device_count, devices.data());

    for (const auto& device : devices)
    {
        auto ssd = get_swapchain_support_details(device, context.v_surface);
        if (is_device_suitable(context, device, ssd))
        {
            context.device.v_physical = device;
            context.device.swapchain_details = ssd;
            break;
        }
    }
    ASSERT_VK(context.device.v_physical != VK_NULL_HANDLE);
}

void poly::vk::create_logical_device(context& context)
{
    queue_families qf = get_queue_families(context.device.v_physical, context.v_surface);

    context.device.graphics_queue_index = qf.graphics.value();
    context.device.present_queue_index = qf.present.value();
    context.device.compute_queue_index = qf.compute.value();

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos {};
    std::set<uint32_t> unique_queue_families = {};
    if (qf.graphics.has_value()) unique_queue_families.insert(qf.graphics.value());
    if (qf.present.has_value()) unique_queue_families.insert(qf.present.value());
    if (qf.compute.has_value()) unique_queue_families.insert(qf.compute.value());
    if (qf.transfer.has_value()) unique_queue_families.insert(qf.transfer.value());

    // Queues for each required capability.
    float queue_priority = 1.0f;
    for(uint32_t qfam : unique_queue_families)
    {
        printf("Creating queue from family #%d\n", qfam);
        VkDeviceQueueCreateInfo q_create_info {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        q_create_info.queueFamilyIndex = qfam;
        q_create_info.queueCount = 1;
        q_create_info.pQueuePriorities = &queue_priority;
        q_create_info.flags = 0;
        queue_create_infos.push_back(q_create_info);
    }
  
    VkPhysicalDeviceFeatures pd_features {};
    VkDeviceCreateInfo device_create_info {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    device_create_info.pQueueCreateInfos = queue_create_infos.data();
  
    device_create_info.pEnabledFeatures = &pd_features;

    device_create_info.enabledExtensionCount = static_cast<uint32_t>(context.requested_device_extensions.size());
    device_create_info.ppEnabledExtensionNames = context.requested_device_extensions.data();

 #ifdef POLYMORPH_VULKAN_USE_VALIDATION
     device_create_info.enabledLayerCount = static_cast<uint32_t>(context.requested_layers.size());
     device_create_info.ppEnabledLayerNames = context.requested_layers.data();
 #endif

     CHECK_VK(vkCreateDevice(context.device.v_physical, &device_create_info, VK_NULL_HANDLE, &context.device.v_logical));
     vkGetDeviceQueue(context.device.v_logical, qf.graphics.value(), 0, &context.device.v_graphics_queue);
     vkGetDeviceQueue(context.device.v_logical, qf.present.value(),  0, &context.device.v_present_queue);
     vkGetDeviceQueue(context.device.v_logical, qf.compute.value(),  0, &context.device.v_compute_queue);
}

void poly::vk::create_vma_allocator(context& context)
{
    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;
    allocator_info.physicalDevice = context.device.v_physical;
    allocator_info.device = context.device.v_logical;
    allocator_info.instance = context.v_instance;
        
    CHECK_VK(vmaCreateAllocator(&allocator_info, &context.allocator));
}