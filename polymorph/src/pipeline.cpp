#include "polymorph/vulkan/context.h"
#include "polymorph/io/file.h"

using namespace poly::vk;

void poly::vk::create_graphics_pipeline(context& context, pipeline& pipeline, graphics_pipeline_spec& spec)
{
	VkPipelineDynamicStateCreateInfo dynamic_state_info{};
	dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(spec.dynamic_states.size());
	dynamic_state_info.pDynamicStates = spec.dynamic_states.data();

	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(spec.vertex_input.vertex_binding_descriptions.size());
	vertex_input_info.pVertexBindingDescriptions = spec.vertex_input.vertex_binding_descriptions.data();
	vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(spec.vertex_input.vertex_attribute_descriptions.size());
	vertex_input_info.pVertexAttributeDescriptions = spec.vertex_input.vertex_attribute_descriptions.data();

	VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
	input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_info.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewport_state_info{};
	viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state_info.viewportCount = static_cast<uint32_t>(spec.viewport.viewports.size());
	viewport_state_info.pViewports = spec.viewport.viewports.data();
	viewport_state_info.scissorCount = static_cast<uint32_t>(spec.viewport.scissors.size());
	viewport_state_info.pScissors = spec.viewport.scissors.data();

	//
	
	VkPipelineRasterizationStateCreateInfo rasterizer_info{};
	rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer_info.depthClampEnable = spec.rasterization.depth_clamp;
	rasterizer_info.rasterizerDiscardEnable = spec.rasterization.discard;
	rasterizer_info.polygonMode = spec.rasterization.polygon_mode;
	rasterizer_info.lineWidth = spec.rasterization.line_width;
	rasterizer_info.cullMode = spec.rasterization.cull_mode;
	rasterizer_info.frontFace = spec.rasterization.front_face;
	rasterizer_info.depthBiasEnable = spec.rasterization.depth_bias.enable;
	rasterizer_info.depthBiasConstantFactor = spec.rasterization.depth_bias.constant_factor;
	rasterizer_info.depthBiasClamp = spec.rasterization.depth_bias.clamp;
	rasterizer_info.depthBiasSlopeFactor = spec.rasterization.depth_bias.slope_factor;

	VkPipelineMultisampleStateCreateInfo multisampling_info{};
	multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling_info.sampleShadingEnable = spec.multisampling.sample_shading;
	multisampling_info.rasterizationSamples = spec.multisampling.raster_samples;
	multisampling_info.minSampleShading = spec.multisampling.min_sample_shading;
	multisampling_info.pSampleMask = spec.multisampling.sample_mask;
	multisampling_info.alphaToCoverageEnable = spec.multisampling.alpha_to_coverage;
	multisampling_info.alphaToOneEnable = spec.multisampling.alpha_to_one;

	std::vector<VkPipelineColorBlendAttachmentState> attachments;
	for (const auto& attachment_spec : spec.color_blend_attachments)
	{
		VkPipelineColorBlendAttachmentState color_blend_attachment_info{};
		color_blend_attachment_info.colorWriteMask = attachment_spec.write_mask;
		color_blend_attachment_info.blendEnable = attachment_spec.enable;
		color_blend_attachment_info.srcColorBlendFactor = attachment_spec.src_color_blend_factor;
		color_blend_attachment_info.dstColorBlendFactor = attachment_spec.dst_color_blend_factor;
		color_blend_attachment_info.colorBlendOp = attachment_spec.color_blend_op;
		color_blend_attachment_info.srcAlphaBlendFactor = attachment_spec.src_alpha_blend_factor;
		color_blend_attachment_info.dstAlphaBlendFactor = attachment_spec.dst_alpha_blend_factor;
		color_blend_attachment_info.alphaBlendOp = attachment_spec.alpha_blend_op;
		attachments.push_back(color_blend_attachment_info);
	}

	VkPipelineColorBlendStateCreateInfo color_blend_state_info{};
	color_blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_state_info.logicOpEnable = spec.color_blend.logic_op_enable;
	color_blend_state_info.logicOp = spec.color_blend.logic_op;
	color_blend_state_info.attachmentCount = static_cast<uint32_t>(attachments.size());
	color_blend_state_info.pAttachments = attachments.data();
	color_blend_state_info.blendConstants[0] = spec.color_blend.blend_consts[0]; 
	color_blend_state_info.blendConstants[1] = spec.color_blend.blend_consts[1]; 
	color_blend_state_info.blendConstants[2] = spec.color_blend.blend_consts[2]; 
	color_blend_state_info.blendConstants[3] = spec.color_blend.blend_consts[3]; 

	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(spec.pipeline_layout.set_layouts.size());
	pipeline_layout_info.pSetLayouts = spec.pipeline_layout.set_layouts.data();
	pipeline_layout_info.pushConstantRangeCount = static_cast<uint32_t>(spec.pipeline_layout.push_const_ranges.size());
	pipeline_layout_info.pPushConstantRanges = spec.pipeline_layout.push_const_ranges.data();

	CHECK_VK(vkCreatePipelineLayout(context.device.v_logical, &pipeline_layout_info, VK_NULL_HANDLE, &pipeline.v_layout));

	std::vector<VkPipelineShaderStageCreateInfo> stage_infos {};
	for (const auto& stage : spec.shader_stages)
	{
		VkPipelineShaderStageCreateInfo info{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
		info.module = stage.module;
		info.stage = stage.stage;
		info.pName = stage.entry.c_str();
		stage_infos.push_back(info);
	}

	VkGraphicsPipelineCreateInfo pipeline_info{};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = static_cast<uint32_t>(stage_infos.size());
	pipeline_info.pStages = stage_infos.data();

	pipeline_info.renderPass = context.v_render_pass;
	pipeline_info.layout = pipeline.v_layout;

	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly_info;
	pipeline_info.pViewportState = &viewport_state_info;
	pipeline_info.pRasterizationState = &rasterizer_info;
	pipeline_info.pMultisampleState = &multisampling_info;
	pipeline_info.pDepthStencilState = nullptr; // Optional
	pipeline_info.pColorBlendState = &color_blend_state_info;
	pipeline_info.pDynamicState = &dynamic_state_info;

	assert(vkCreateGraphicsPipelines(context.device.v_logical, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline.v_pipeline) == VK_SUCCESS);

	for (const auto& stage : spec.shader_stages)
	{
		vkDestroyShaderModule(context.device.v_logical, stage.module, nullptr);
	}
}

void poly::vk::create_raytracing_pipeline(context&, pipeline&)
{

}

VkShaderModule poly::vk::create_shader_module(VkDevice device, const std::vector<char>& source)
{
	VkShaderModuleCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize = source.size();
	info.pCode = reinterpret_cast<const uint32_t*>(source.data());

	VkShaderModule module;
	CHECK_VK(vkCreateShaderModule(device, &info, nullptr, &module));

	return module;
}


void poly::vk::destroy_pipeline(context& context, pipeline& pipeline)
{
	vkDestroyPipelineLayout(context.device.v_logical, pipeline.v_layout, VK_NULL_HANDLE);
	vkDestroyPipeline(context.device.v_logical, pipeline.v_pipeline, VK_NULL_HANDLE);
}

graphics_pipeline_spec graphics_pipeline_spec::default(context& context)
{
	graphics_pipeline_spec spec = {};

	spec.dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	spec.vertex_input = { {}, {} };
	spec.input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	spec.input_assembly.primitive_restart = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)context.swapchain.v_extent.width;
	viewport.height = (float)context.swapchain.v_extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = context.swapchain.v_extent;

	spec.viewport.viewports.push_back(viewport);
	spec.viewport.scissors.push_back(scissor);

	spec.rasterization.depth_clamp = VK_FALSE;
	spec.rasterization.discard = VK_FALSE;
	spec.rasterization.polygon_mode = VK_POLYGON_MODE_FILL;
	spec.rasterization.line_width = 1.0f;
	spec.rasterization.cull_mode = VK_CULL_MODE_BACK_BIT;
	spec.rasterization.front_face = VK_FRONT_FACE_CLOCKWISE;
	spec.rasterization.depth_bias.enable = VK_FALSE;
	spec.rasterization.depth_bias.constant_factor = 0.0f;
	spec.rasterization.depth_bias.clamp = 0.0f;
	spec.rasterization.depth_bias.slope_factor = 0.0f;

	spec.multisampling.sample_shading = VK_FALSE;
	spec.multisampling.raster_samples = VK_SAMPLE_COUNT_1_BIT;
	spec.multisampling.min_sample_shading = 1.0f;
	spec.multisampling.sample_mask = nullptr;
	spec.multisampling.alpha_to_coverage = VK_FALSE;
	spec.multisampling.alpha_to_one = VK_FALSE;

	color_blend_attachment attachment{};
	attachment.write_mask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	attachment.enable = VK_FALSE;
	attachment.src_color_blend_factor = VK_BLEND_FACTOR_ONE;
	attachment.dst_color_blend_factor = VK_BLEND_FACTOR_ZERO;
	attachment.color_blend_op = VK_BLEND_OP_ADD;
	attachment.src_alpha_blend_factor = VK_BLEND_FACTOR_ONE;
	attachment.dst_alpha_blend_factor = VK_BLEND_FACTOR_ZERO;
	attachment.alpha_blend_op = VK_BLEND_OP_ADD;
	spec.color_blend_attachments.push_back(attachment);

	spec.color_blend.logic_op_enable = VK_FALSE;
	spec.color_blend.logic_op = VK_LOGIC_OP_COPY;
	spec.color_blend.blend_consts[0] = 0.0f;
	spec.color_blend.blend_consts[1] = 0.0f;
	spec.color_blend.blend_consts[2] = 0.0f;
	spec.color_blend.blend_consts[3] = 0.0f;

	spec.pipeline_layout.set_layouts = {};
	spec.pipeline_layout.push_const_ranges = {};

	shader_stage stage_vertex {};
	stage_vertex.entry = "main";
	stage_vertex.module = create_shader_module(context.device.v_logical, read_file_vec_u8("shader/test.vert.spv"));
	stage_vertex.stage = VK_SHADER_STAGE_VERTEX_BIT;
	spec.shader_stages.push_back(stage_vertex);

	shader_stage stage_fragment{};
	stage_fragment.entry = "main";
	stage_fragment.module = create_shader_module(context.device.v_logical, read_file_vec_u8("shader/test.frag.spv"));
	stage_fragment.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	spec.shader_stages.push_back(stage_fragment);
	
	return spec;
}