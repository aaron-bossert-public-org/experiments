
#pragma once

#include "igpu/texture/draw_target.h"

#include <memory>

namespace igpu
{
	class vulkan_render_target;
	class vulkan_depth_target;

	class vulkan_draw_target : public draw_target
	{
	public:

		struct config : draw_target::config
		{
			struct vulkan
			{
				vulkan_render_target* color = nullptr;
				vulkan_depth_target* depth = nullptr;
			};

			vulkan vk;
		};

		const config& cfg() const override;

		static std::unique_ptr<vulkan_draw_target> make(
			const config&);

	private:

		vulkan_draw_target(
			const config&);

		const config _cfg;
	};
}
