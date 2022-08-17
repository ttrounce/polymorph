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
    /// @brief A wrapper for a vulkan buffer and its allocation.
    struct buffer // buffer.cpp
    {
        VkBuffer value;
        VmaAllocation allocation;
    };

    /// @brief A wrapper for a vulkan image and its allocation.
    struct image // image.cpp
    {
        VkImage        v_image;
        VkImageView    v_view;
        VkDeviceMemory v_memory;
    };

    /// @brief A wrapper for a vulkan buffer and its allocation.
    struct framebuffer // framebuffer.cpp
    {
        VkFramebuffer buf;
    };

    /// @brief A wrapper for a vulkan swapchain, and associated objects and informations.
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

    /// @brief A wrapper for a vulkan command buffer.
    struct command_buffer
    {
        VkCommandBuffer buf;
    };

    /// @brief A wrapper for a vulkan logical and physical device, along with a command pool and queue information.
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

    /// @brief The central context for any vulkan related function.
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
        
        /*! @brief Initializes the context.
        *   @param[in] app_name The application name provided to the vulkan instance.
        *   @param[in] requested_layers The requested validation layers to be used with this vulkan instance.
        *   @param[in] requested_device_extensions The requested device extensions to be used with this vulkan instance and the logical device.
        *   @param[in] glfw_window The GLFW window handle.
        */
        void init(const std::string& app_name, const std::vector<const char*>& requested_layers, std::vector<const char*>& requested_device_extensions, GLFWwindow* glfw_window);

        /*! @brief Cleans up all context-bound vulkan objects.
        *   @note Some other wrapper objects will require specific destruction.
        */
        void cleanup();
    };

    /// @brief A struct containing configuration details to create a raster graphics pipeline.
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

        /*! @brief Creates a pre-configured graphics pipeline config.
        *   @static
        *   @sa @ref pipeline
        *   @param[in] context The associated vulkan context wrapper.
        *   @since Indev
        */
        static gfx_pipeline_cfg default(const context& context);
    };

    /// @brief A wrapper for several synchronization objects pertaining to drawing frames.
    struct synchron // sync.cpp
    {
        std::vector<VkSemaphore> semas_image_available;
        std::vector<VkSemaphore> semas_render_finished;
        std::vector<VkFence>     fences_in_flight;
    };

    /// @brief A wrapper for a vulkan pipeline.
    struct pipeline // pipeline.cpp
    {
        VkPipeline          v_pipeline;
        VkPipelineLayout    v_layout;
        VkPipelineBindPoint v_bind_point;
    };

    /// @brief A collection of command buffers.
    struct command_buffer_set // commands.cpp
    {
        std::vector<command_buffer> values;
    };

    /// @brief A collection of states required to draw frames.
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

    /*! @brief Creates the vulkan instance for the context.
    *   @memberof context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @since Indev
    */
    void create_instance(context& context);              
    
    /*! @brief Creates the debug messenger for the given context.
    *   @memberof context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @since Indev
    */
    void create_debug_messenger(context& context);

    /*! @brief Destroys the debug messenger for the given context.
    *   @memberof context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @since Indev
    */
    void destroy_debug_messenger(context& context);       

    /*! @brief Creates a vulkan surface for the given context and window.
    *   @memberof context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @since Indev
    */
    void create_surface(context& context);

    /*! @brief Retrieves a physical device to populate the @ref device wrapper.
    *   @memberof device
    *   @sa @ref context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @since Indev
    */ 
    void create_physical_device(context& context);          

    /*! @brief Creates a logical device to populate the @ref device wrapper.
    *   @memberof device
    *   @sa @ref context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @since Indev
    */
    void create_logical_device(context& context);  

    /*! @brief Creates a VMA allocator object.
    *   @memberof device
    *   @sa @ref context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @since Indev
    */
    void create_vma_allocator(context& context);

    /*! @brief Creates the context swapchain wrapper and populates its contents.
    *   @memberof swapchain
    *   @sa @ref context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @since Indev
    */
    void create_swapchain(context& context);

    /*! @brief Recreates the context swapchain and repopulates the wrapper contents.
    *   @note Blocks while the window is minimized.
    *   @memberof swapchain
    *   @sa @ref context
    *   @since Indev
    */
    void recreate_swapchain(context& context);

    /*! @brief Destroys the context swapchain wrapper and its contents.
    *   @memberof swapchain
    *   @sa @ref context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @since Indev
    */
    void destroy_swapchain(context& context);

    /*! @brief Creates images views for the swapchain images of the given context.
    *   @memberof swapchain
    *   @sa @ref image
    *   @sa @ref context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @since Indev
    */
    void create_swap_image_views(context& context);              

    /*! @brief Creates the default render pass for the given context.
    *   @memberof context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @since Indev
    */
    void create_render_pass(context& context);

    /*! @brief Creates the swapchain framebuffers for the given context.
    *   @memberof swapchain
    *   @sa @ref framebuffer
    *   @sa @ref context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @since Indev
    */
    void create_swap_framebuffers(context& context);

    /*! @brief Creates a command pool for the given context, used to produce command buffers.
    *   @memberof device
    *   @sa command_buffer
    *   @sa @ref context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @since Indev
    */
    void create_command_pool(context& context);

//  ----- Generic -----

    /*! @brief Allocates and stores data to a buffer using staging.
    *   @memberof buffer
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] buf The buffer wrapper to allocate and store to.
    *   @param[in] size The amount of bytes to allocate and store.
    *   @param[in] usage The VkBufferUsageFlags to specify the usages of the buffer.
    *   @param[in] memory_props The properties of the memory allocated.
    *   @since Indev
    */
    void create_staged_buffer(const context&     context,
                              buffer&            buffer,
                              VkDeviceSize       size,
                              VkBufferUsageFlags usage,
                              const void*        input_data);

    /*! @brief Allocates an empty buffer.
    *   @memberof buffer
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] buf The buffer wrapper to allocate for.
    *   @param[in] size The amount of bytes to allocate for.
    *   @param[in] usage The VkBufferUsageFlags to specify the usages of the buffer.
    *   @param[in] memory_props The properties of the memory allocated.
    *   @since Indev
    */
    void create_buffer(const context&        context,
                       buffer&               buf,
                       VkDeviceSize          size,
                       VkBufferUsageFlags    usage,
                       VkMemoryPropertyFlags memory_props);

    /*! @brief Stores data to a pre-allocated buffer.
    *   @memberof buffer
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in] buf The buffer wrapper containing the memory handle to write to.
    *   @param[in] data The data to write to the buffer.
    *   @since Indev
    */
    void store_buffer(const context& context,
                      const buffer&  buf,
                      const void*    data);

    /*! @brief Copies data from one buffer handle to another.
    *   @memberof buffer
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in] src The buffer to copy from.
    *   @param[out] dst The buffer to copy to.
    *   @param[in] size The amount of bytes to copy over.
    *   @since Indev
    */
    void copy_buffer(const context& context,
                     VkBuffer       src,
                     VkBuffer       dst,
                     VkDeviceSize   size);

    /*! @brief Unallocates the buffer and frees any memory.
    *   @memberof buffer
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] buf The buffer wrapper to destroy the contents of and free any allocated memory.
    *   @since Indev
    */
    void destroy_buffer(const context& context,
                        buffer&        buf);

    /*! @brief Creates a new vulkan framebuffer handle with a given attachments and dimensions.
    *   @memberof framebuffer
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] framebuffer The framebuffer wrapper to create and populate.
    *   @param[in] render_pass The render pass linked with the framebuffer.
    *   @param[in] attachments The image view attachments to be used in a render pass with this framebuffer.
    *   @param[in] dimensions The dimensions of the framebuffer.
    *   @since Indev
    */
    void create_framebuffer(const context&                  context,
                            framebuffer&                    framebuffer,
                            VkRenderPass                    render_pass,
                            const std::vector<VkImageView>& attachments,
                            const glm::uvec3&               dimensions);

    /*! @brief Destroys a vulkan framebuffer handle.
    *   @memberof framebuffer
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] framebuffer The framebuffer wrapper to destroy the contents of.
    *   @since Indev
    */
    void destroy_framebuffer(const context& context,
                             framebuffer& framebuffer);
                 
    /*! @brief Creates a vulkan image and stores it in the image wrapper.
    *   @memberof image
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] img The image wrapper to store the image in.
    *   @todo Implement.
    *   @since Indev
    */
    void create_image(const context& context,
                      image&         img); // TODO: implement

    /*! @brief Creates a view to an image with a given format and flag settings.
    *   @memberof image
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] img The image wrapper to store the image view in.
    *   @param[in] format The image view format.
    *   @param[in] flags The image aspect flags.
    *   @since Indev
    */
    void create_image_view(const context&     context,
                           image&             img,
                           VkFormat           format,
                           VkImageAspectFlags flags);

    /*! @brief Destroys a vulkan image and empties the image wrapper.
    *   @memberof image
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] img The image wrapper to destroy the contents of.
    *   @since Indev
    */
    void destroy_image(const context& context,
                       image&         img);
     
    /*! @brief Creates a specified number of command buffers using the given settingsand stores them in the provided command buffer set.
    *   @memberof command_buffer_set
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] command_buffer_set The command buffer set to create and populate.
    *   @param[in] buffer_level The VkCommandBufferLevel to be used in allocating each command buffer.
    *   @param[in] count The amount of command buffers to create.
    *   @since Indev
    */
    void create_command_buffer_set(const context&       context,
                                   command_buffer_set&  command_buffer_set,
                                   VkCommandBufferLevel buffer_level,
                                   uint32_t             count);

    /*! @brief Starts the recording of vkCmd functions into the provided command buffer.
    *   @related command_buffer
    *   @param[in] command_buffer The command buffer to begin recording to.
    *   @since Indev
    */
    void begin_recording_commands(const command_buffer& command_buffer); 

    /*! @brief Ends the recording of vkCmd functions into the provided command buffer.
    *   @related command_buffer
    *   @param[in] command_buffer The command buffer to end recording to.
    *   @since Indev
    */
    void end_recording_commands(const command_buffer& command_buffer);

    /*! @brief Begins the specified render pass using a commandbuffer.
    *   @related command_buffer
    *   @param[in] command_buffer The command buffer to write the command to.
    *   @param[in] render_pass The render pass to begin.
    *   @param[in] framebuffer The framebuffer to write to.
    *   @param[in] extend The render extent (dimensions).
    *   @since Indev
    */
    void begin_render_pass(const command_buffer& command_buffer,
                           VkRenderPass          render_pass,
                           const framebuffer&    framebuffer,
                           const VkExtent2D&     extent);

    /*! @brief Ends the render pass.
    *   @related command_buffer
    *   @param[in] command_buffer The command buffer to write the command to.
    *   @since Indev
    */
    void end_render_pass(const command_buffer& command_buffer);

    /*! @brief Acquires the next image from the swapchain to begin the next frame.
    *   @related draw_state_context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @param[in,out] draw_state_context The struct holding relevant information and state used to draw a frame.
    *   @since Indev
    */
    void begin_frame(context&            context,
                     draw_state_context& draw_state_context);

    /*! @brief Submits the queue and presents the produced image.
    *   @related draw_state_context
    *   @param[in,out] context The associated vulkan context wrapper.
    *   @param[in,out] draw_state_context The struct holding relevant information and state used to draw a frame.
    *   @since Indev
    */
    void end_frame(context&            context,
                   draw_state_context& draw_state_context);

    /*! @brief Creates vulkan synchronization objects needed to draw frames and populates the given sync object.
    *   @memberof synchron
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] sync The sync object wrapper to create and populate.
    *   @param[in] max_syncs The number of each type of semaphore and fence to create.
    *   @since Indev
    */
    void create_synchron(const context& context,
                         synchron&      sync,
                         uint32_t       max_syncs);

    /*! @brief Destroys vulkan synchronization objects in the given sync objectand resets handles to VK_NULL_HANDLE.
    *   @memberof synchron
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] sync The sync object wrapper to destroy the contents of.
    *   @since Indev
    */
    void destroy_synchron(const context& context,
                          synchron&      sync);

    /*! @brief Creates a vulkan rasterization pipeline using the provided configuration.
    *   @related pipeline
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] pipeline The pipeline wrapper to create.
    *   @param[in] spec The configuration for this raster graphics pipeline.
    *   @since Indev
    */
    void create_graphics_pipeline(const context&          context,
                                  pipeline&               pipeline,
                                  const gfx_pipeline_cfg& spec);                  

    /*! @brief Creates a vulkan raytracing pipeline.
    *   @related pipeline
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] pipeline The pipeline wrapper to create.
    *   @todo Implement.
    */
    void create_raytracing_pipeline(const context& context,
                                    pipeline&      pipeline); 

    /*! @brief Destroys a vulkan pipelineand resets handles to VK_NULL_HANDLE.
    *   @memberof pipeline
    *   @param[in] context The associated vulkan context wrapper.
    *   @param[in,out] pipeline The pipeline wrapper to destroy the contents of.
    *   @since Indev
    */
    void destroy_pipeline(const context& context,
                          pipeline&      pipeline);

    /*! @brief Creates a VKShaderModule handle using the given SPIR-V source
    *   @return A VkShaderModule handle.
    *   @param[in] device A VkDevice handle.
    *   @param[in] source A vector of characters containing SPIR-V source.
    *   @since Indev 
    */
    VkShaderModule create_shader_module(VkDevice                 device,
                                        const std::vector<char>& source);
}