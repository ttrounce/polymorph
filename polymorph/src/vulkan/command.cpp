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

void poly::vk::create_command_buffer_set(const context& context, command_buffer_set& buffers, VkCommandBufferLevel level, uint32_t count)
{
	VkCommandBufferAllocateInfo info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	info.commandPool = context.device.v_command_pool;
	info.level = level;
	info.commandBufferCount = count;

	std::vector<VkCommandBuffer> bufs;
	bufs.resize(count);
	CHECK_VK(vkAllocateCommandBuffers(context.device.v_logical, &info, bufs.data()));

	for (auto buf : bufs)
	{
		buffers.values.push_back(command_buffer{ buf });
	}
}

void poly::vk::begin_recording_commands(const command_buffer& cmd_buf)
{
	VkCommandBufferBeginInfo info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	info.flags = 0;
	info.pInheritanceInfo = VK_NULL_HANDLE;

	CHECK_VK(vkBeginCommandBuffer(cmd_buf.buf, &info));
}

void poly::vk::end_recording_commands(const command_buffer& cmd_buf)
{
	CHECK_VK(vkEndCommandBuffer(cmd_buf.buf));
}

void poly::vk::begin_render_pass(const command_buffer& cmd_buf, VkRenderPass pass, const framebuffer& framebuffer, const VkExtent2D& extent)
{
	VkRenderPassBeginInfo info{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	info.renderPass = pass;
	info.framebuffer = framebuffer.buf;
	info.renderArea.offset = { 0, 0 }; // TODO: make configurable
	info.renderArea.extent = extent;

	VkClearValue clear{};
	clear.color = { 0.0f, 0.0f, 0.0f, 1.0f }; // TODO: make configurable
	clear.depthStencil = {};
	info.clearValueCount = 1;
	info.pClearValues = &clear;

	vkCmdBeginRenderPass(cmd_buf.buf, &info, VK_SUBPASS_CONTENTS_INLINE); // TODO: make configurable VK_SUBPASS_CONTENTS_???
}

void poly::vk::end_render_pass(const command_buffer& cmd_buf)
{
	vkCmdEndRenderPass(cmd_buf.buf);
}

void poly::vk::begin_frame(context& context, draw_state_context& dsc)
{
	vkWaitForFences(context.device.v_logical, 1, &dsc.sync.fences_in_flight[dsc.current_frame], VK_TRUE, UINT64_MAX);

	VkResult result = vkAcquireNextImageKHR(context.device.v_logical, context.swapchain.v_swapchain, UINT64_MAX, dsc.sync.semas_image_available[dsc.current_frame], VK_NULL_HANDLE, &dsc.current_image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreate_swapchain(context);
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		printf("Swapchain out of date, unable to acquire a new image\n");
		throw std::runtime_error("Swapchain out of date, unable to acquire a new image");
	}

	vkResetFences(context.device.v_logical, 1, &dsc.sync.fences_in_flight[dsc.current_frame]);

	vkResetCommandBuffer(dsc.command_buffers.values[dsc.current_frame].buf, 0);
}

void poly::vk::end_frame(context& context, draw_state_context& dsc)
{
	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	std::vector<VkCommandBuffer> raw_bufs;
	for (auto& cb : dsc.command_buffers.values)
	{
		raw_bufs.push_back(cb.buf);
	}

	VkSemaphore wait_semaphores[] = { dsc.sync.semas_image_available[dsc.current_frame] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &raw_bufs[dsc.current_frame];
	VkSemaphore signal_semaphores[] = { dsc.sync.semas_render_finished[dsc.current_frame] };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	CHECK_VK(vkQueueSubmit(context.device.v_graphics_queue, 1, &submit_info, dsc.sync.fences_in_flight[dsc.current_frame]));

	VkPresentInfoKHR present_info{};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;

	VkSwapchainKHR swapChains[] = { context.swapchain.v_swapchain };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapChains;
	present_info.pImageIndices = &dsc.current_image_index;
	present_info.pResults = nullptr; // Optional

	VkResult result = vkQueuePresentKHR(context.device.v_present_queue, &present_info);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		recreate_swapchain(context);
	}
	else if (result != VK_SUCCESS)
	{
		printf("Swapchain out of date, failed to present image\n");
		throw std::runtime_error("Swapchain out of date, failed to present image");
	}

	dsc.current_frame = (dsc.current_frame + 1) % dsc.max_frames;
}