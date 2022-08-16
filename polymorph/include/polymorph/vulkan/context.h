#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>
#include <vector>
#include <glm/vec3.hpp>

#include "utility.h"

#include <vk_mem_alloc.h>

namespace poly::vk
{
    struct buffer // buffer.cpp
    {
        VkBuffer value;
        VmaAllocation allocation;
    };

    struct image // image.cpp
    {
        VkImage        v_image;
        VkImageView    v_view;
        VkDeviceMemory v_memory;
    };

    struct framebuffer // framebuffer.cpp
    {
        VkFramebuffer buf;
        // std::vector<VkImageView> attachments; // consider refs/pointers?
        // VkRenderPass render_pass;
    };

    struct swapchain // swapchain.cpp
    {
        VkSwapchainKHR           v_swapchain;
                                 
        VkSurfaceFormatKHR       v_surface_format;
        VkPresentModeKHR         v_present_mode;
        VkExtent2D               v_extent;

        std::vector<VkImage>     images;
        std::vector<VkImageView> image_views;

        std::vector<framebuffer> framebuffers;

        uint32_t max_frames_in_flight = 2;
    };

    struct command_buffer
    {
        VkCommandBuffer buf;
    };

    struct device // device.cpp
    {
        VkDevice                  v_logical;
        VkPhysicalDevice          v_physical;
        VkCommandPool             v_command_pool;
                                  
        uint32_t                  graphics_queue_index;
        uint32_t                  present_queue_index;
        uint32_t                  compute_queue_index;
                                  
        VkQueue                   v_graphics_queue;
        VkQueue                   v_present_queue;
        VkQueue                   v_compute_queue;

        swapchain_support_details swapchain_details;
    };

    struct context // context.cpp
    {
        VkInstance               v_instance;
        VkDebugUtilsMessengerEXT v_debug_messenger;
        VkSurfaceKHR             v_surface;
        VkRenderPass             v_render_pass; // TODO: consider configurability

        device                   device {};
        swapchain                swapchain {};

        VmaAllocator             allocator;

        std::string              app_name;
        GLFWwindow*              glfw_window;
        std::vector<const char*> requested_layers;
        std::vector<const char*> requested_device_extensions;
        
        void init(const std::string& app_name, const std::vector<const char*>& requested_layers, std::vector<const char*>& requested_device_extensions, GLFWwindow* glfw_window);
        void cleanup();
    };

    struct gfx_pipeline_cfg // pipeline.cpp
    {
        std::vector<VkDynamicState> dynamic_states;

        struct
        {
            std::vector<VkVertexInputBindingDescription>   vertex_binding_descriptions;
            std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions;
        } vertex_input;

        struct
        {
            VkPrimitiveTopology topology;
            VkBool32            primitive_restart;
        } input_assembly;

        struct
        {
            std::vector<VkViewport> viewports;
            std::vector<VkRect2D>   scissors;
        } viewport;

        struct
        {
            struct
            {
                VkBool32 enable;
                float    constant_factor;
                float    clamp;
                float    slope_factor;
            } depth_bias;
            VkBool32        depth_clamp;
            VkBool32        discard;
            VkPolygonMode   polygon_mode;
            float           line_width;
            VkCullModeFlags cull_mode;
            VkFrontFace     front_face;
        } rasterization;

        struct
        {
            VkBool32 sample_shading;
            VkSampleCountFlagBits raster_samples;
            float min_sample_shading;
            VkSampleMask* sample_mask;
            VkBool32 alpha_to_coverage;
            VkBool32 alpha_to_one;
        } multisampling;

        struct color_blend_attachment
        {
            VkColorComponentFlags write_mask;
            VkBool32              enable;
            VkBlendFactor         src_color_blend_factor;
            VkBlendFactor         dst_color_blend_factor;
            VkBlendOp             color_blend_op;
            VkBlendFactor         src_alpha_blend_factor;
            VkBlendFactor         dst_alpha_blend_factor;
            VkBlendOp             alpha_blend_op;
        };
        std::vector<color_blend_attachment> color_blend_attachments;

        struct
        {
            VkBool32  logic_op_enable;
            VkLogicOp logic_op;
            float     blend_consts[4];
        } color_blend;

        struct
        {
            std::vector<VkDescriptorSetLayout> set_layouts;
            std::vector<VkPushConstantRange>   push_const_ranges;
        } pipeline_layout;

        struct shader_stage
        {
            VkShaderModule        module;
            VkShaderStageFlagBits stage;
            std::string           entry;
        };
        std::vector<shader_stage> shader_stages;

        static gfx_pipeline_cfg default(context&);
    };

    struct synchron // sync.cpp
    {
        std::vector<VkSemaphore> semas_image_available;
        std::vector<VkSemaphore> semas_render_finished;
        std::vector<VkFence>     fences_in_flight;
    };

    struct pipeline // pipeline.cpp
    {
        VkPipeline          v_pipeline;
        VkPipelineLayout    v_layout;
        VkPipelineBindPoint v_bind_point;
    };

    struct command_buffer_set // commands.cpp
    {
        std::vector<command_buffer> values;
    };

    struct draw_state_context
    {
        pipeline&           pipeline;
        synchron&           sync;
        command_buffer_set& command_buffers;

        uint32_t            max_frames = 0;

        uint32_t            current_frame = 0;
        uint32_t            current_image_index = 0;
    };
                  
//  ----- Contextual -----

    void create_instance          (context& context);                                    
    void create_debug_messenger   (context& context);                                      
    void destroy_debug_messenger  (context& context);                                      
    void create_surface           (context& context);

    void create_physical_device   (context& context);                                    
    void create_logical_device    (context& context);  
    void create_vma_allocator     (context& context);

    void create_swapchain         (context& context);        
    void recreate_swapchain       (context& context);
    void destroy_swapchain        (context& context);
    void create_swap_image_views  (context& context);              
    void create_render_pass       (context& context);
    void create_swap_framebuffers (context& context);

    void create_command_pool      (context& context);

//  ----- Generic -----

//  buffer.cpp
    void create_staged_buffer(
        const context& context,
        buffer& buf,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        const void* input_data);

    void create_buffer(
        const context& context,
        buffer& buf,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags memory_props);

    void store_buffer(
        const context& context,
        const buffer& buf,
        const void* data);

    void copy_buffer(
        const context& context,
        VkBuffer src,
        VkBuffer dst,
        VkDeviceSize size);

    void destroy_buffer(
        const context& context,
        const buffer& buf);

//  framebuffer.cpp
    void create_framebuffer(
        context& context,
        framebuffer& framebuffer,
        VkRenderPass renderpass,
        const std::vector<VkImageView>& attachments,
        glm::uvec3 dimensions);

    void destroy_framebuffer(
        context& context,
        framebuffer& framebuffer);
                 
//  image.cpp
    void create_image(
        context& context,
        image& img); // TODO: implement

    void create_image_view(
        context& context,
        image& img,
        VkFormat format,
        VkImageAspectFlags flags);

    void destroy_image(
        context& context,
        image& img);
     
//  command.cpp
    void create_command_buffer_set(
        context& context,
        command_buffer_set& command_buffer_set,
        VkCommandBufferLevel buffer_level,
        uint32_t count);

    void begin_recording_commands(
        command_buffer& command_buffer); 

    void end_recording_commands(
        command_buffer& command_buffer);

    void begin_render_pass(
        command_buffer& command_buffer,
        VkRenderPass renderpass,
        const framebuffer& framebuffer,
        const VkExtent2D& extent);

    void end_render_pass(
        command_buffer& command_buffer);

    void begin_frame(
        context& context, 
        draw_state_context& draw_state_context);

    void end_frame(
        context& context,
        draw_state_context& draw_state_context);

    void create_synchron(
        context& context,
        synchron& sync,
        uint32_t max_syncs);

    void destroy_synchron(
        context& context,
        synchron& sync);

//  pipeline.cpp
    void create_graphics_pipeline(
        context& context,
        pipeline& pipeline,
        gfx_pipeline_cfg& spec);                  

    void create_raytracing_pipeline(
        context& context,
        pipeline& pipeline); // TODO: implement

    void destroy_pipeline(
        context& context,
        pipeline& pipeline);

    VkShaderModule create_shader_module(
        VkDevice device,
        const std::vector<char>& source);
}