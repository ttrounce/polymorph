#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>
#include <vector>
#include <glm/vec3.hpp>

#include "utility.h"

namespace poly::vk
{
    struct image // image.cpp
    {
        VkImage        v_image;
        VkImageView    v_view;
        VkDeviceMemory v_memory;
    };

    struct framebuffer
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

        command_buffer           graphics_command_buffer; // Q: could this be generalised?

        void init(const std::string& app_name, const std::vector<const char*>& requested_layers, std::vector<const char*>& requested_device_extensions, GLFWwindow* glfw_window);
        void cleanup();

        std::string              app_name;
        GLFWwindow*              glfw_window;
        std::vector<const char*> requested_layers;
        std::vector<const char*> requested_device_extensions;
    };

    struct graphics_pipeline_spec // pipeline.cpp
    {
        std::vector<VkDynamicState> dynamic_states;

        struct
        {
            std::vector<VkVertexInputBindingDescription> vertex_binding_descriptions;
            std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions;
        } vertex_input;

        struct
        {
            VkPrimitiveTopology topology;
            VkBool32 primitive_restart;
        } input_assembly;

        struct
        {
            std::vector<VkViewport> viewports;
            std::vector<VkRect2D> scissors;
        } viewport;

        struct
        {
            VkBool32 depth_clamp;
            VkBool32 discard;
            VkPolygonMode polygon_mode;
            float line_width;
            VkCullModeFlags cull_mode;
            VkFrontFace front_face;
            struct
            {
                VkBool32 enable;
                float constant_factor;
                float clamp;
                float slope_factor;
            } depth_bias;
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
            VkBool32 enable;
            VkBlendFactor src_color_blend_factor;
            VkBlendFactor dst_color_blend_factor;
            VkBlendOp color_blend_op;
            VkBlendFactor src_alpha_blend_factor;
            VkBlendFactor dst_alpha_blend_factor;
            VkBlendOp alpha_blend_op;
        };
        std::vector<color_blend_attachment> color_blend_attachments;

        struct
        {
            VkBool32 logic_op_enable;
            VkLogicOp logic_op;
            float blend_consts[4];
        } color_blend;

        struct
        {
            std::vector<VkDescriptorSetLayout> set_layouts;
            std::vector<VkPushConstantRange> push_const_ranges;
        } pipeline_layout;

        struct shader_stage
        {
            VkShaderModule module;
            VkShaderStageFlagBits stage;
            std::string entry;
        };
        std::vector<shader_stage> shader_stages;

        static graphics_pipeline_spec default(context&);
    };

    struct pipeline // pipeline.cpp
    {
        VkPipeline       v_pipeline;
        VkPipelineLayout v_layout;
    };

    // void create_context_instance();
                  
    // instance.cpp, core
    void create_instance(context&);                                      
    void create_debug_messenger(context&);                                      
    void destroy_debug_messenger(context&);                                      
    void create_surface(context&);

    // device.cpp, core
    void create_physical_device(context&);                                      
    void create_logical_device(context&);                                      
                
    // swapchain.cpp, core
    void create_swapchain(context&); 
    void create_swap_image_views(context&);              
    void create_render_pass(context&);
    void create_swap_framebuffers(context&);

    // command.cpp, core
    void create_command_pool(context&);
    void create_graphics_command_buffer(context&);

    // framebuffer.cpp, generic
    void create_framebuffer(context&, framebuffer&, VkRenderPass, const std::vector<VkImageView>, glm::uvec3);
    void destroy_framebuffer(context&, framebuffer&);
                 
    // image.cpp, generic
    void create_image(context&); // TODO: implement
    void create_image_view(context&, image&, VkFormat, VkImageAspectFlags);
    void destroy_image(context&, image&);                              
             
    // pipeline.cpp, generic
    void create_graphics_pipeline(context&, pipeline&, graphics_pipeline_spec&); // creates gfx pipeline to a spec                        
    void create_raytracing_pipeline(context&, pipeline&); // TODO: implement
    void destroy_pipeline(context&, pipeline&);
    VkShaderModule create_shader_module(VkDevice, const std::vector<char>&);
}