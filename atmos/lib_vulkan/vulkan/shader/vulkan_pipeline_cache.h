
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/shader/vulkan_graphics_pipeline.h"

#include <memory>
#include <unordered_map>
namespace igpu
{
	class vulkan_back_buffer;

	class vulkan_pipeline_cache
	{
	public:
		struct config
		{
			struct vulkan
			{
				VkDevice device = nullptr;
			};

			vulkan vk;
		};

		void on_back_buffer_resized( const vulkan_back_buffer& );

		std::shared_ptr< vulkan_graphics_pipeline > memoized(
			const graphics_pipeline::config& );

		static std::unique_ptr< vulkan_pipeline_cache > make( const config& );

		~vulkan_pipeline_cache();

	private:
		vulkan_pipeline_cache( const config& );

	private:
		const config _cfg;
		VkPipelineCache _cache;

		std::unordered_map<
			VkPipeline,
			std::weak_ptr< vulkan_graphics_pipeline > >
			_live_pipelines;
	};
}
