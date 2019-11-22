
#include "vulkan/shader/vulkan_graphics_pipeline.h"

#include "vulkan/shader/vulkan_fragment_shader.h"
#include "vulkan/shader/vulkan_pipeline_cache.h"
#include "vulkan/shader/vulkan_program.h"
#include "vulkan/shader/vulkan_render_states.h"
#include "vulkan/shader/vulkan_vertex_shader.h"
#include "vulkan/sync/vulkan_abandon_manager.h"
#include "vulkan/texture/vulkan_draw_target.h"

using namespace igpu;

namespace
{
	struct vertex_assembly
	{
		std::array<
			VkVertexInputBindingDescription,
			vertex_parameters::MAX_COUNT >
			binding_descriptions;

		std::array<
			VkVertexInputAttributeDescription,
			vertex_parameters::MAX_COUNT >
			attribute_descriptions;

		VkPipelineVertexInputStateCreateInfo vertex_input_info;
		VkPipelineInputAssemblyStateCreateInfo input_assembly;
	};

	void build_vertex_assembly(
		VkPrimitiveTopology vk_topology,
		const vulkan_vertex_parameters& parameters,
		const std::vector< vertex_buffer::config >& compact_vertex_format,
		vertex_assembly* out_assembly )
	{
		out_assembly->vertex_input_info = {
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			(VkPipelineVertexInputStateCreateFlags)0,
			(uint32_t)compact_vertex_format.size(),
			out_assembly->binding_descriptions.data(),
			0,
			out_assembly->attribute_descriptions.data(),
		};

		out_assembly->input_assembly = {
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			(VkPipelineInputAssemblyStateCreateFlags)0,
			vk_topology,
			!"primitiveRestartEnable",
		};

		uint32_t* p_attrib_index =
			&out_assembly->vertex_input_info.vertexAttributeDescriptionCount;

		for ( size_t binding = 0; binding < compact_vertex_format.size();
			  ++binding )
		{
			const vertex_buffer::config& buff_cfg =
				compact_vertex_format[binding];

			out_assembly->binding_descriptions[binding] = {
				(uint32_t)binding,
				buff_cfg.stride,
				VK_VERTEX_INPUT_RATE_VERTEX,
			};

			for ( size_t a = 0; a < buff_cfg.attributes.size();
				  ++a, ++*p_attrib_index )
			{
				const vulkan_vertex_parameter& parameter =
					parameters.parameter( *p_attrib_index );

				const vertex_buffer::attribute& attribute =
					buff_cfg.attributes[a];

				out_assembly->attribute_descriptions[*p_attrib_index] = {
					(uint32_t)parameter.cfg().location,
					(uint32_t)binding,
					parameter.cfg().vk.format,
					(uint32_t)attribute.offset,
				};
			}
		}

		ASSERT_CONTEXT(
			parameters.count() ==
				out_assembly->vertex_input_info.vertexAttributeDescriptionCount,
			"incorrect number of parameters(%d), program expects(%d)",
			out_assembly->vertex_input_info.vertexAttributeDescriptionCount,
			(int)parameters.count() );
	}

	VkPipeline build_pipeline( const vulkan_graphics_pipeline::config& cfg )
	{
		if ( cfg.topology == topology::UNDEFINED )
		{
			LOG_CRITICAL( "topology is undefined" );
		}
		else if ( !cfg.vk.device )
		{
			LOG_CRITICAL( "device is null" );
		}
		else if ( !cfg.vk.pipeline_cache )
		{
			LOG_CRITICAL( "pipeline_cache is null" );
		}
		else if ( !cfg.vk.program )
		{
			LOG_CRITICAL( "program is null" );
		}
		else if ( !cfg.vk.render_states )
		{
			LOG_CRITICAL( "render_states is null" );
		}
		else if ( !cfg.vk.render_states )
		{
			LOG_CRITICAL( "render_states is null" );
		}
		else if ( !cfg.vk.draw_target )
		{
			LOG_CRITICAL( "draw_target is null" );
		}
		else
		{
			auto res = cfg.draw_target->res();

			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = (float)res.y;
			viewport.width = (float)res.x;
			viewport.height = (float)-res.y;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor = {};
			scissor.offset = {};
			scissor.extent = {
				(uint32_t)res.x,
				(uint32_t)res.y,
			};

			VkPipelineViewportStateCreateInfo viewport_state = {};
			viewport_state.sType =
				VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewport_state.viewportCount = 1;
			viewport_state.pViewports = &viewport;
			viewport_state.scissorCount = 1;
			viewport_state.pScissors = &scissor;

			VkPipelineRasterizationStateCreateInfo rasterizer = {};
			rasterizer.sType =
				VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = cfg.vk.render_states->cfg().vk.cull;
			rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;

			VkPipelineMultisampleStateCreateInfo multisampling = {};
			multisampling.sType =
				VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples =
				cfg.vk.draw_target->cfg().vk.color->cfg().vk.sample_count;

			VkPipelineColorBlendStateCreateInfo color_blending = {};
			color_blending.sType =
				VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			color_blending.logicOpEnable = VK_FALSE;
			color_blending.logicOp = VK_LOGIC_OP_COPY;
			color_blending.attachmentCount = 1;
			color_blending.pAttachments =
				&cfg.vk.render_states->cfg().vk.color_blend;
			color_blending.blendConstants[0] = 0.0f;
			color_blending.blendConstants[1] = 0.0f;
			color_blending.blendConstants[2] = 0.0f;
			color_blending.blendConstants[3] = 0.0f;

			std::array< VkPipelineShaderStageCreateInfo, 2 > shader_stages = {
				cfg.vk.program->cfg().vk.vertex->stage_info(),
				cfg.vk.program->cfg().vk.fragment->stage_info(),
			};

			vertex_assembly assembly;
			build_vertex_assembly(
				cfg.vk.topology,
				cfg.vk.program->vertex_parameters(),
				cfg.compact_vertex_format,
				&assembly );

			VkGraphicsPipelineCreateInfo pipeline_info = {
				VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			};

			pipeline_info.stageCount = (uint32_t)shader_stages.size();
			pipeline_info.pStages = shader_stages.data();
			pipeline_info.pVertexInputState = &assembly.vertex_input_info;
			pipeline_info.pInputAssemblyState = &assembly.input_assembly;
			pipeline_info.pViewportState = &viewport_state;
			pipeline_info.pRasterizationState = &rasterizer;
			pipeline_info.pMultisampleState = &multisampling;
			pipeline_info.pDepthStencilState =
				&cfg.vk.render_states->cfg().vk.depth_stencil;
			pipeline_info.pColorBlendState = &color_blending;
			pipeline_info.layout = cfg.vk.program->pipeline_layout();
			pipeline_info.renderPass = cfg.vk.draw_target->render_pass();
			pipeline_info.subpass =
				(uint32_t)cfg.vk.draw_target->raster_sub_pass();
			pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

			VkPipeline pipeline = nullptr;
			vkCreateGraphicsPipelines(
				cfg.vk.device,
				cfg.vk.pipeline_cache->vk_pipeline_cache(),
				1,
				&pipeline_info,
				nullptr,
				&pipeline );
			return pipeline;
		}

		return nullptr;
	}
}
const vulkan_graphics_pipeline::config& vulkan_graphics_pipeline::cfg() const
{
	return _cfg;
}

VkPipeline vulkan_graphics_pipeline::vk_pipeline() const
{
	return _vk_pipeline;
}

void vulkan_graphics_pipeline::rebind_draw_target(
	const scoped_ptr< vulkan_draw_target >& draw_target )
{
	if ( draw_target )
	{
		_cfg.draw_target = scoped_ptr< igpu::draw_target >( draw_target );
		_cfg.vk.draw_target = draw_target;

		abandon( _cfg.vk.queue, _cfg.vk.device, _vk_pipeline );
		_vk_pipeline = build_pipeline( _cfg );
	}
}

std::unique_ptr< vulkan_graphics_pipeline > vulkan_graphics_pipeline::make(
	const config& cfg )
{
	if ( VkPipeline pipeline = build_pipeline( cfg ) )
	{
		return std::unique_ptr< vulkan_graphics_pipeline >(
			new vulkan_graphics_pipeline( cfg, pipeline ) );
	}
	return nullptr;
}

vulkan_graphics_pipeline::~vulkan_graphics_pipeline()
{
	abandon( _cfg.vk.queue, _cfg.vk.device, _vk_pipeline );
}

vulkan_graphics_pipeline::vulkan_graphics_pipeline(
	const config& cfg,
	VkPipeline vk_pipeline )
	: _cfg( cfg )
	, _vk_pipeline( vk_pipeline )
{}
