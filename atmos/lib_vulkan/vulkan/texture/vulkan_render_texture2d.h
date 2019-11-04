
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/texture/vulkan_render_target.h"

#include "igpu/texture/render_texture2d.h"

namespace igpu
{
	class vulkan_image;

	class vulkan_render_texture2d
		: public render_texture2d
		, public vulkan_render_target
	{
	public:
		struct config : render_texture2d::config
		{
			vulkan vk;
		};

		virtual const config& cfg() const = 0;

		virtual vulkan_image& gpu_resource() = 0;

		virtual const vulkan_image& gpu_resource() const = 0;

		static std::unique_ptr< vulkan_render_texture2d > make( const config& );
	};
}
