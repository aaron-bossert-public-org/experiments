
#include "vulkan/compute/vulkan_compute_pipeline.h"

#include "vulkan/compute/vulkan_compute_program.h"
#include "vulkan/compute/vulkan_compute_shader.h"
#include "vulkan/manager/vulkan_abandon_manager.h"
#include "vulkan/shader/vulkan_pipeline_cache.h"
#include "vulkan/shader/vulkan_specialization_constants.h"

using namespace igpu;

namespace
{
	VkPipeline build_pipeline( const vulkan_compute_pipeline::config& cfg )
	{
		if ( !cfg.vk.device )
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
		else
		{
			vulkan_specialization_constants specializations(
				cfg.compact_constants,
				shader_stages::COMPUTE );

			VkComputePipelineCreateInfo pipeline_info = {
				VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			};

			pipeline_info.layout = cfg.vk.program->pipeline_layout();
			pipeline_info.stage =
				cfg.vk.program->cfg().vk.compute->stage_info();
			pipeline_info.stage.pSpecializationInfo = &specializations.info();

			VkPipeline pipeline = nullptr;
			vkCreateComputePipelines(
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
const vulkan_compute_pipeline::config& vulkan_compute_pipeline::cfg() const
{
	return _cfg;
}

VkPipeline vulkan_compute_pipeline::vk_pipeline() const
{
	return _vk_pipeline;
}

std::unique_ptr< vulkan_compute_pipeline > vulkan_compute_pipeline::make(
	const config& cfg )
{
	if ( VkPipeline pipeline = build_pipeline( cfg ) )
	{
		return std::unique_ptr< vulkan_compute_pipeline >(
			new vulkan_compute_pipeline( cfg, pipeline ) );
	}
	return nullptr;
}

vulkan_compute_pipeline::~vulkan_compute_pipeline()
{
	abandon( _cfg.vk.queue, _cfg.vk.device, _vk_pipeline );
}

vulkan_compute_pipeline::vulkan_compute_pipeline(
	const config& cfg,
	VkPipeline vk_pipeline )
	: _cfg( cfg )
	, _vk_pipeline( vk_pipeline )
{}
