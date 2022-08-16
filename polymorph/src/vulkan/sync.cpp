#include "polymorph/vulkan/context.h"

using namespace poly::vk;

void poly::vk::create_synchron(context& context, synchron& syncs, uint32_t count)
{
	syncs.semas_image_available.resize(count);
	syncs.semas_render_finished.resize(count);
	syncs.fences_in_flight.resize(count);

	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < count; i++)
	{
		CHECK_VK(vkCreateSemaphore(context.device.v_logical, &semaphore_info, nullptr, &syncs.semas_image_available[i]));
		CHECK_VK(vkCreateSemaphore(context.device.v_logical, &semaphore_info, nullptr, &syncs.semas_render_finished[i]));
		CHECK_VK(vkCreateFence(context.device.v_logical, &fence_info, nullptr, &syncs.fences_in_flight[i]));
	}
}

void poly::vk::destroy_synchron(context& context, synchron& sync)
{
	for (auto& sema : sync.semas_image_available)
	{
		vkDestroySemaphore(context.device.v_logical, sema, VK_NULL_HANDLE);
	}
	for (auto& sema : sync.semas_render_finished)
	{
		vkDestroySemaphore(context.device.v_logical, sema, VK_NULL_HANDLE);
	}
	for (auto& fence : sync.fences_in_flight)
	{
		vkDestroyFence(context.device.v_logical, fence, VK_NULL_HANDLE);
	}
}
