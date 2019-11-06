
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
				VkDevice* device = nullptr;
			};

			vulkan vk;
		};

		// std::shared_ptr< vulkan_graphics_pipeline > memoize_graphics_pipeine(
		//	vulkan_program&,
		//	vulkan_render_states&,
		//	vulkan_geometry& );

		void back_buffer_resized( const vulkan_back_buffer& );

		static std::unique_ptr< vulkan_graphics_pipeline > make(
			const config& );

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
