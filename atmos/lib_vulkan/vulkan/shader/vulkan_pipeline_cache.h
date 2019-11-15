
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include <memory>
#include <unordered_map>
namespace igpu
{
	class attribute_indexer;
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

		VkPipelineCache vk_pipeline_cache() const;

		static std::unique_ptr< vulkan_pipeline_cache > make( const config& );

		~vulkan_pipeline_cache();

	private:
		vulkan_pipeline_cache( const config&, VkPipelineCache );

	private:
		const config _cfg;
		VkPipelineCache _vk_pipeline_cache;
	};
}
