
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/shader/graphics_pipeline.h"

#include "framework/utility/scoped_ptr.h"

#include "glm/vec2.hpp"

namespace igpu
{
	class vulkan_queue;
	class vulkan_draw_target;
	class vulkan_pipeline_cache;
	class vulkan_program;
	class vulkan_render_states;

	class vulkan_graphics_pipeline : public graphics_pipeline
	{
	public:
		struct config : graphics_pipeline::config
		{
			struct vulkan
			{
				VkDevice device;
				scoped_ptr< vulkan_queue > queue;
				scoped_ptr< vulkan_pipeline_cache > pipeline_cache;
				scoped_ptr< vulkan_draw_target > draw_target;
				scoped_ptr< vulkan_program > program;
				scoped_ptr< vulkan_render_states > render_states;
				VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
			};

			vulkan vk;
		};

		const config& cfg() const override;

		VkPipeline vk_pipeline() const;

		void rebind_draw_target( const scoped_ptr< vulkan_draw_target >& );

		static std::unique_ptr< vulkan_graphics_pipeline > make(
			const config& );

		~vulkan_graphics_pipeline();

	private:
		vulkan_graphics_pipeline( const config&, VkPipeline );

	private:
		config _cfg;
		VkPipeline _vk_pipeline;
	};
}
