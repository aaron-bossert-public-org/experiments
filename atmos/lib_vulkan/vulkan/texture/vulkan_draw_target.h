
#pragma once

#include "vulkan/texture/vulkan_depth_buffer.h"
#include "vulkan/texture/vulkan_render_buffer.h"

#include "igpu/texture/draw_target.h"

#include "framework/utility/scoped_ptr.h"

#include <memory>

namespace igpu
{
	class vulkan_command_buffer;

	class vulkan_draw_target : public draw_target
	{
	public:
		struct config : draw_target::config
		{
			struct vulkan
			{
				VkDevice device = nullptr;
				std::shared_ptr< vulkan_render_buffer > color = nullptr;
				std::shared_ptr< vulkan_depth_buffer > depth = nullptr;
			};

			vulkan vk;
		};

		const config& cfg() const override;

		void begin_raster() override;

		scoped_ptr< vulkan_command_buffer > raster_cmds();

		void end_raster() override;

		uint32_t raster_sub_pass() const;

		VkRenderPass render_pass() const;

		static std::unique_ptr< vulkan_draw_target > make( const config& );

		~vulkan_draw_target();

	protected:
		vulkan_draw_target( const config& );

	private:
		const config _cfg;
		uint32_t _raster_sub_pass = UINT32_MAX;
		VkRenderPass _render_pass = nullptr;
		std::shared_ptr< vulkan_command_buffer > _raster_cmds;
	};
}
