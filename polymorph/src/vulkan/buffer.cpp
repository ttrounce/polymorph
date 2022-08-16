#include "polymorph/vulkan/context.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

using namespace poly::vk;

void poly::vk::create_staged_buffer(const context& context, buffer& buf, VkDeviceSize size, VkBufferUsageFlags usage, const void*  input_data)
{
 	buffer staging {};
	create_buffer(context, staging, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	store_buffer(context, staging, input_data);

	create_buffer(context, buf, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	copy_buffer(context, staging.value, buf.value, size);

	destroy_buffer(context, staging);
}

void poly::vk::create_buffer(const context& context, buffer& buf, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_props)
{
	VkBufferCreateInfo buf_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	buf_info.size = size;
	buf_info.usage = usage;
	buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: make configurable

	VmaAllocationCreateInfo vma_allocation_info{};
	vma_allocation_info.requiredFlags = memory_props;
	vma_allocation_info.flags = memory_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ? VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT : 0;
	vma_allocation_info.usage = VMA_MEMORY_USAGE_AUTO;
	vmaCreateBuffer(context.allocator, &buf_info, &vma_allocation_info, &buf.value, &buf.allocation, VK_NULL_HANDLE);
}

void poly::vk::store_buffer(const context& context, const buffer& buf, const void* input_data)
{
	void* data = 0;
	vmaMapMemory(context.allocator, buf.allocation, &data);
	memcpy(data, input_data, static_cast<size_t>(buf.allocation->GetSize()));
	vmaUnmapMemory(context.allocator, buf.allocation);
}

void poly::vk::copy_buffer(const context& context, VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo alloc_info { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = context.device.v_command_pool;
	alloc_info.commandBufferCount = 1;
	alloc_info.pNext = NULL;

	VkCommandBuffer buf;
	vkAllocateCommandBuffers(context.device.v_logical, &alloc_info, &buf);

	VkCommandBufferBeginInfo begin_info { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO }; // recording
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(buf, &begin_info);

	VkBufferCopy copy_region{};
	copy_region.srcOffset = 0; // Optional
	copy_region.dstOffset = 0; // Optional
	copy_region.size = size;
	vkCmdCopyBuffer(buf, src, dst, 1, &copy_region);

	vkEndCommandBuffer(buf);

	// execute transfer

	VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &buf;

	vkQueueSubmit(context.device.v_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(context.device.v_graphics_queue);

	vkFreeCommandBuffers(context.device.v_logical, context.device.v_command_pool, 1, &buf);
}

void poly::vk::destroy_buffer(const context& context, buffer& buf)
{
	vmaDestroyBuffer(context.allocator, buf.value, buf.allocation);
	buf.allocation = VK_NULL_HANDLE;
	buf.value = VK_NULL_HANDLE;
}