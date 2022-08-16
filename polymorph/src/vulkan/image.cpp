#include "polymorph/vulkan/context.h"

using namespace poly::vk;

// ------------------------- IMAGE -------------------------

void poly::vk::create_image_view(const context& context, image& image, VkFormat format, VkImageAspectFlags aspect_flags)
{
    VkImageViewCreateInfo info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    info.format = format;
    info.image = image.v_image;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: make configurable
    info.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }; // TODO: make configurable

    info.subresourceRange.aspectMask = aspect_flags;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;

    CHECK_VK(vkCreateImageView(context.device.v_logical, &info, VK_NULL_HANDLE, &image.v_view));
}

void poly::vk::destroy_image(const context& context, image& image)
{
    if (image.v_view != VK_NULL_HANDLE)
    {
        vkDestroyImageView(context.device.v_logical, image.v_view, VK_NULL_HANDLE);
        image.v_view = VK_NULL_HANDLE;
    }
    if (image.v_image != VK_NULL_HANDLE)
    {
        vkDestroyImage(context.device.v_logical, image.v_image, VK_NULL_HANDLE);
        image.v_image = VK_NULL_HANDLE;
    }
    if (image.v_memory != VK_NULL_HANDLE)
    {
        vkFreeMemory(context.device.v_logical, image.v_memory, VK_NULL_HANDLE);
        image.v_memory = VK_NULL_HANDLE;
    }
}