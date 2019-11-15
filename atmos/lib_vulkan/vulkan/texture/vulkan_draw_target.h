
#pragma once

#include "vulkan/texture/vulkan_depth_buffer.h"
#include "vulkan/texture/vulkan_render_buffer.h"

#include "igpu/texture/draw_target.h"

#include <memory>

namespace igpu
{
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

		const vulkan_render_buffer& color() const override;

		const vulkan_depth_buffer& depth() const override;

		VkRenderPass render_pass() const;

		static std::unique_ptr< vulkan_draw_target > make( const config& );

		~vulkan_draw_target();

	protected:
		vulkan_draw_target( const config& );

	private:
		const config _cfg;
		VkRenderPass _render_pass = nullptr;
	};
}
