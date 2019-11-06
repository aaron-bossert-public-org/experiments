
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_gpu_object.h"

#include "igpu/shader/graphics_pipeline.h"

namespace igpu
{
	class vulkan_program;
	class vulkan_render_states;

	class vulkan_graphics_pipeline
		: public graphics_pipeline
		, public vulkan_gpu_object
	{
	public:
		struct config : graphics_pipeline::config
		{
			struct vulkan
			{
				std::shared_ptr< vulkan_program > program;
				std::shared_ptr< vulkan_render_states > render_states;
				VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
			};

			vulkan vk;
		};

		const config& cfg() const override;

		state& object_state() override;

		static std::unique_ptr< vulkan_graphics_pipeline > make(
			const config& );

		~vulkan_graphics_pipeline();

	private:
		vulkan_graphics_pipeline( const config& );

	private:
		const config _cfg;
		state _object_state;
	};
}
