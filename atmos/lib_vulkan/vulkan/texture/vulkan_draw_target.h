
#pragma once

#include "vulkan/texture/vulkan_depth_buffer.h"
#include "vulkan/texture/vulkan_render_buffer.h"

#include "igpu/texture/draw_target.h"

#include "framework/utility/scoped_ptr.h"

#include <memory>

namespace igpu
{
	class vulkan_command_buffer;
	class vulkan_managers;
	class vulkan_poset_fence;
	class vulkan_queue;

	class vulkan_draw_target : public draw_target
	{
	public:
		struct config : draw_target::config
		{
			struct vulkan
			{
				VkDevice device = nullptr;
				scoped_ptr< vulkan_managers > managers;
				std::shared_ptr< vulkan_render_buffer > color = nullptr;
				std::shared_ptr< vulkan_depth_buffer > depth = nullptr;
				size_t swap_count = 0;
			};

			vulkan vk;
		};

		const config& cfg() const override;

		virtual void begin_raster() = 0;

		virtual scoped_ptr< vulkan_command_buffer > raster_cmds() = 0;

		virtual scoped_ptr< vulkan_poset_fence > raster_fence() const = 0;

		virtual void end_raster() = 0;

		virtual size_t swap_index() const = 0;

		const scoped_ptr< vulkan_queue >& raster_queue() const;

		uint32_t raster_sub_pass() const;

		VkRenderPass render_pass() const;

		static std::unique_ptr< vulkan_draw_target > make( const config& );

		~vulkan_draw_target();

	protected:
		struct state
		{
			uint32_t raster_sub_pass = UINT32_MAX;
			VkRenderPass render_pass = nullptr;
		};

		vulkan_draw_target( const config& );

	private:
		const config _cfg;
		const state _state;
	};
}
