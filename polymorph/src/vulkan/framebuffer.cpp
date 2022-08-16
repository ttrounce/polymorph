#include "polymorph/vulkan/context.h"

using namespace poly::vk;

void poly::vk::create_framebuffer(context& context, framebuffer& framebuffer, VkRenderPass render_pass, const std::vector<VkImageView>& attachments, glm::uvec3 dimensions)
{
    VkFramebufferCreateInfo info{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    info.renderPass = render_pass;
    info.attachmentCount = static_cast<uint32_t>(attachments.size());
    info.pAttachments = attachments.data();
    info.width = dimensions.x;
    info.height = dimensions.y;
    info.layers = dimensions.z;

    CHECK_VK(vkCreateFramebuffer(context.device.v_logical, &info, VK_NULL_HANDLE, &framebuffer.buf))
}

void poly::vk::destroy_framebuffer(context& context, framebuffer& framebuffer)
{
    vkDestroyFramebuffer(context.device.v_logical, framebuffer.buf, VK_NULL_HANDLE);
    framebuffer.buf = VK_NULL_HANDLE; // TODO: do this for all types
;}