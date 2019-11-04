

#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/texture/vulkan_depth_target.h"

#include "igpu/texture/depth_texture2d.h"

namespace igpu
{
	class vulkan_image;

	class vulkan_depth_texture2d
		: public depth_texture2d
		, public vulkan_depth_target
	{
	public:
		struct config : depth_texture2d::config
		{
			vulkan vk;
		};

		virtual const config& cfg() const = 0;

		static std::unique_ptr< vulkan_depth_texture2d > make( const config& );
	};
}
