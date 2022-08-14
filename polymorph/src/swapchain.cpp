#include "polymorph/vulkan/context.h"
#include "polymorph/vulkan/utility.h"
#include "polymorph/vulkan/defines.h"

#include <limits> 
#include <algorithm> 

using namespace poly::vk;

// ------------------------- UTILS -------------------------

static VkSurfaceFormatKHR get_swap_surface_format(context& context, const std::vector<VkSurfaceFormatKHR>& formats)
{
	for (const auto& available : formats) {
		if (available.format == VK_FORMAT_B8G8R8A8_SRGB && available.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return available;
		}
	}
	return formats.front();
}

static VkPresentModeKHR get_swap_present_mode(context& context, const std::vector<VkPresentModeKHR>& modes)
{
	for (const auto& available : modes) {
		if (available == VK_PRESENT_MODE_MAILBOX_KHR) {
			return available;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D get_swap_extent(context& context, const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(context.glfw_window, &width, &height);

		VkExtent2D actual_extent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actual_extent;
	}
}

// ------------------------- SWAPCHAIN -------------------------

void poly::vk::create_swapchain(context& context)
{
	auto ssd = get_swapchain_support_details(context.device.v_physical, context.v_surface);

	auto surface_format = get_swap_surface_format(context, ssd.formats);
	auto present_mode = get_swap_present_mode(context, ssd.present_modes);
	auto extent = get_swap_extent(context, ssd.capabilities);

	uint32_t image_count = ssd.capabilities.minImageCount + 1;
	if (ssd.capabilities.maxImageCount > 0 && image_count > ssd.capabilities.maxImageCount)
	{
		image_count = ssd.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR create_info { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	create_info.surface = context.v_surface;

	// swapchain image details
	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // Used as a colour attachment

	if (context.device.graphics_queue_index != context.device.present_queue_index)
	{
		uint32_t indices[] = { context.device.graphics_queue_index, context.device.present_queue_index };
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = indices;
	}
	else
	{
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = VK_NULL_HANDLE;
	}

	create_info.preTransform = ssd.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	context.swapchain.v_extent = extent;
	context.swapchain.v_present_mode = present_mode;
	context.swapchain.v_surface_format = surface_format;
	CHECK_VK(vkCreateSwapchainKHR(context.device.v_logical, &create_info, nullptr, &context.swapchain.v_swapchain));

	vkGetSwapchainImagesKHR(context.device.v_logical, context.swapchain.v_swapchain, &image_count, VK_NULL_HANDLE);
	context.swapchain.images.resize(image_count);
	vkGetSwapchainImagesKHR(context.device.v_logical, context.swapchain.v_swapchain, &image_count, context.swapchain.images.data());
}

void poly::vk::create_swap_image_views(context& context)
{
	context.swapchain.image_views.resize(context.swapchain.images.size());
	
	for (size_t i = 0; i < context.swapchain.images.size(); i++)
	{
		image dummy_image {}; // Use a dummy object
		dummy_image.v_image = context.swapchain.images[i]; // Give it the image

		create_image_view(context, dummy_image, context.swapchain.v_surface_format.format, VK_IMAGE_ASPECT_COLOR_BIT);

		context.swapchain.image_views[i] = dummy_image.v_view; // Extract view
	}
}