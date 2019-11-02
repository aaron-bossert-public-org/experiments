
#pragma once

#include <igpu/texture/depth_buffer.h>

#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/texture/vulkan_depth_target.h>

namespace igpu
{
	VkFormat to_vulkan_format(depth_format format);

	class vulkan_image;

	class vulkan_depth_buffer : public depth_buffer, public vulkan_depth_target
	{

	public:

		struct config : depth_buffer::config
		{
			vulkan vk;
		};

		virtual const config& cfg() const = 0;

		virtual vulkan_image& gpu_resource() = 0;

		virtual const vulkan_image& gpu_resource() const = 0;

		static std::unique_ptr<vulkan_depth_buffer> make(
			const config&);
	};
}

