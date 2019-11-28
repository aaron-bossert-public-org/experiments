
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/compute/compute_pipeline.h"

#include "framework/utility/scoped_ptr.h"

#include "glm/vec2.hpp"
#include <memory>

namespace igpu
{
	class vulkan_queue;
	class vulkan_pipeline_cache;
	class vulkan_compute_program;

	class vulkan_compute_pipeline : public compute_pipeline
	{
	public:
		struct config : compute_pipeline::config
		{
			struct vulkan
			{
				VkDevice device;
				scoped_ptr< vulkan_queue > queue;
				scoped_ptr< vulkan_pipeline_cache > pipeline_cache;
				scoped_ptr< vulkan_compute_program > program;
			};

			vulkan vk;
		};

		const config& cfg() const override;

		VkPipeline vk_pipeline() const;

		static std::unique_ptr< vulkan_compute_pipeline > make( const config& );

		~vulkan_compute_pipeline();

	private:
		vulkan_compute_pipeline( const config&, VkPipeline );

	private:
		config _cfg;
		VkPipeline _vk_pipeline;
	};
}
