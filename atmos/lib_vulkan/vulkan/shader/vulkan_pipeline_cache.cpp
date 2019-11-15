
#include "vulkan/shader/vulkan_pipeline_cache.h"

#include "vulkan/window/vulkan_back_buffer.h"

using namespace igpu;


VkPipelineCache vulkan_pipeline_cache::vk_pipeline_cache() const
{
	return _vk_pipeline_cache;
}

std::unique_ptr< vulkan_pipeline_cache > vulkan_pipeline_cache::make(
	const config& cfg )
{
	if ( !cfg.vk.device )
	{
		LOG_CRITICAL( "device is null" );
	}
	else
	{
		VkPipelineCacheCreateInfo pipeline_cache_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		};

		VkPipelineCache vk_pipeline_cache = nullptr;
		vkCreatePipelineCache(
			cfg.vk.device,
			&pipeline_cache_create_info,
			nullptr,
			&vk_pipeline_cache );

		if ( vk_pipeline_cache )
		{
			return std::unique_ptr< vulkan_pipeline_cache >(
				new vulkan_pipeline_cache( cfg, vk_pipeline_cache ) );
		}
	}

	return nullptr;
}

vulkan_pipeline_cache ::~vulkan_pipeline_cache()
{
	vkDestroyPipelineCache( _cfg.vk.device, _vk_pipeline_cache, nullptr );
}

vulkan_pipeline_cache::vulkan_pipeline_cache(
	const config& cfg,
	VkPipelineCache vk_pipeline_cache )
	: _cfg( cfg )
	, _vk_pipeline_cache( vk_pipeline_cache )
{}