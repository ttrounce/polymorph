#include "polymorph/vulkan/context.h"

using namespace poly::vk;

void poly::vk::create_command_pool(context& context)
{
	queue_families qfi = get_queue_families(context.device.v_physical, context.v_surface);

	VkCommandPoolCreateInfo info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	info.queueFamilyIndex = qfi.graphics.value_or(0); // TODO: revise queue family method

	CHECK_VK(vkCreateCommandPool(context.device.v_logical, &info, VK_NULL_HANDLE, &context.device.v_command_pool));
}

void poly::vk::create_graphics_command_buffer(context& context)
{
	VkCommandBufferAllocateInfo info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	info.commandPool = context.device.v_command_pool;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	info.commandBufferCount = 1;

	CHECK_VK(vkAllocateCommandBuffers(context.device.v_logical, &info, &context.graphics_command_buffer.buf));
}