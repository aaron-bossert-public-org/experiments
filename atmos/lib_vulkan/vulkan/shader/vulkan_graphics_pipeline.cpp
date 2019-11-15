
#include "vulkan/shader/vulkan_graphics_pipeline.h"

#include "vulkan/shader/vulkan_program.h"
#include "vulkan/shader/vulkan_render_states.h"

using namespace igpu;

namespace
{
	bool build_pipelines(
		const vulkan_graphics_pipeline::config& cfg,
		const glm::ivec2& screen_res,
		VkPipeline* p_fullscreen,
		VkPipeline* p_partscreen )
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
		else if ( !screen_res.x )
		{
			LOG_CRITICAL( "screen_res.x is zero" );
		}
		else if ( !screen_res.y )
		{
			LOG_CRITICAL( "screen_res.y is zero" );
		}
		else
		{
			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)screen_res.x;
			viewport.height = (float)screen_res.x;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor = {};
			scissor.offset = {};
			scissor.extent = {
				(uint32_t)screen_res.x,
				(uint32_t)screen_res.y,
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
			rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;

			VkPipelineMultisampleStateCreateInfo multisampling = {};
			multisampling.sType =
				VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = cfg.vk.msaa_samples;

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


			VkGraphicsPipelineCreateInfo pipeline_info = {
				VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			};

			pipeline_info.stageCount = (uint32_t)shader_stages.size();
			pipeline_info.pStages = shader_stages.data();
			pipeline_info.pVertexInputState =
				&attributes_descriptor.vertex_input_info();
			pipeline_info.pInputAssemblyState =
				&attributes_descriptor.input_assembly();
			pipeline_info.pViewportState = &viewport_state;
			pipeline_info.pRasterizationState = &rasterizer;
			pipeline_info.pMultisampleState = &multisampling;
			pipeline_info.pDepthStencilState =
				render_states_impl.depth_stencil();
			pipeline_info.pColorBlendState = &color_blending;
			pipeline_info.layout = program_impl.pipeline_layout;
			pipeline_info.renderPass = _context->back_buffer().render_pass();
			;
			pipeline_info.subpass = 0;
			pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

			vkCreateGraphicsPipelines(
				_device,
				VK_NULL_HANDLE,
				1,
				&pipeline_info,
				nullptr,
				&_graphics_pipeline );

			return true;
		}

		return false;
	}
}
const vulkan_graphics_pipeline::config& vulkan_graphics_pipeline::cfg() const
{
	return _cfg;
}

std::unique_ptr< vulkan_graphics_pipeline > vulkan_graphics_pipeline::make(
	const config& cfg,
	const glm::ivec2& screen_res )
{
	VkPipeline full_screen_pipeline = nullptr;
	VkPipeline part_screen_pipeline = nullptr;
	if ( !build_pipelines(
			 cfg,
			 screen_res,
			 &full_screen_pipeline,
			 &part_screen_pipeline ) )
	{
		return std::unique_ptr< vulkan_graphics_pipeline >(
			new vulkan_graphics_pipeline(
				cfg,
				screen_res,
				full_screen_pipeline,
				part_screen_pipeline ) );
	}
	return nullptr;
}

vulkan_graphics_pipeline::~vulkan_graphics_pipeline()
{}

vulkan_graphics_pipeline::vulkan_graphics_pipeline(
	const config& cfg,
	const glm::ivec2& screen_res,
	VkPipeline full_screen_pipeline,
	VkPipeline part_screen_pipeline )
	: _cfg( cfg )
	, _screen_res( screen_res )
	, _full_screen_pipeline( full_screen_pipeline )
	, _part_screen_pipeline( part_screen_pipeline )
{}


#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/shader/graphics_pipeline.h"

namespace igpu
{
	class vulkan_program;
	class vulkan_render_states;

	class vulkan_graphics_pipeline : public graphics_pipeline
	{
	public:
		struct config : graphics_pipeline::config
		{
			struct vulkan
			{
				std::shared_ptr< vulkan_program > program;
				std::shared_ptr< vulkan_render_states > render_states;
			};

			vulkan vk;
		};

		const config& cfg() const override;

		VkPipeline vk_pipeline() const;

		static std::unique_ptr< vulkan_graphics_pipeline > make(
			const config& );

		~vulkan_graphics_pipeline();

	private:
		vulkan_graphics_pipeline( const config& );

	private:
		const config _cfg;
	};
}
