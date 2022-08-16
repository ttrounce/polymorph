#include <polymorph/polymorph.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

constexpr const char* WINDOW_TITLE = "Example";

struct vertex
{
    glm::vec2 pos;
    glm::vec3 color;
};

int main()
{
    poly::window<int> window(800, 600, WINDOW_TITLE, 0);

    auto required_layers = std::vector<const char*> { "VK_LAYER_KHRONOS_validation" };
    auto required_device_extensions = std::vector<const char*> { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    poly::vk::context context;
    context.init(WINDOW_TITLE, required_layers, required_device_extensions, window.handle);

    auto gfx_cfg = poly::vk::gfx_pipeline_cfg::default(context);
    
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    gfx_cfg.vertex_input.vertex_binding_descriptions.push_back(binding);
    
    VkVertexInputAttributeDescription attributes[2];
    attributes[0].binding = 0;
    attributes[0].location = 0;
    attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributes[0].offset = offsetof(vertex, pos);
    gfx_cfg.vertex_input.vertex_attribute_descriptions.push_back(attributes[0]);
    attributes[1].binding = 0;
    attributes[1].location = 1;
    attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributes[1].offset = offsetof(vertex, color);
    gfx_cfg.vertex_input.vertex_attribute_descriptions.push_back(attributes[1]);

    poly::vk::pipeline pipeline;
    poly::vk::create_graphics_pipeline(context, pipeline, gfx_cfg);
    
    poly::vk::synchron sync;
    poly::vk::create_synchron(context, sync, context.swapchain.max_frames_in_flight);
    
    poly::vk::command_buffer_set cb_set;
    poly::vk::create_command_buffer_set(context, cb_set, VK_COMMAND_BUFFER_LEVEL_PRIMARY, context.swapchain.max_frames_in_flight);

    poly::vk::draw_state_context dsc{ pipeline, sync, cb_set, context.swapchain.max_frames_in_flight };

    std::vector<vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    };
    poly::vk::buffer vertex_buf;
    poly::vk::create_staged_buffer(context, vertex_buf, sizeof(vertices[0]) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertices.data());

    std::vector<uint16_t> indices = {
        0, 1, 2
    };
    poly::vk::buffer index_buf;
    poly::vk::create_staged_buffer(context, index_buf, sizeof(indices[0]) * indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indices.data());


    window.start([&]()
        {
            poly::vk::begin_frame(context, dsc);
            auto current_cmd = dsc.command_buffers.values[dsc.current_frame];
            {
                poly::vk::begin_recording_commands(current_cmd);
                poly::vk::begin_render_pass(current_cmd, context.v_render_pass, context.swapchain.framebuffers[dsc.current_image_index], context.swapchain.v_extent);

                auto extent = context.swapchain.v_extent;

                std::vector<VkRect2D> scissors{
                    {{0, 0}, extent}
                };
                vkCmdSetScissor(current_cmd.buf, 0, static_cast<uint32_t>(scissors.size()), scissors.data());

                std::vector<VkViewport> viewports{
                    { 0.0f, 0.0f, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, 1.0f }
                };
                vkCmdSetViewport(current_cmd.buf, 0, static_cast<uint32_t>(viewports.size()), viewports.data());

                vkCmdBindPipeline(current_cmd.buf, dsc.pipeline.v_bind_point, dsc.pipeline.v_pipeline);

                VkBuffer raw_vertex_bufs[] = {vertex_buf.value};
                VkDeviceSize raw_offsets[] = {0};
                vkCmdBindVertexBuffers(current_cmd.buf, 0, 1, raw_vertex_bufs, raw_offsets);
                vkCmdBindIndexBuffer(current_cmd.buf, index_buf.value, 0, VK_INDEX_TYPE_UINT16);

                vkCmdDrawIndexed(current_cmd.buf, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

                poly::vk::end_render_pass(current_cmd);
                poly::vk::end_recording_commands(current_cmd);
            }
            poly::vk::end_frame(context, dsc);
        }
    );

    vkDeviceWaitIdle(context.device.v_logical);

    poly::vk::destroy_buffer(context, vertex_buf);
    poly::vk::destroy_buffer(context, index_buf);

    poly::vk::destroy_pipeline(context, pipeline);
    poly::vk::destroy_synchron(context, sync);
    context.cleanup();

    return 0;
}