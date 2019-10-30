
#pragma once

#include <framework/utility/scoped_ptr.h>

#include <igpu/texture/texture2d.h>

#include <vulkan/defines/vulkan_includes.h>

namespace igpu
{
	VkFormat to_vulkan_format(texture_format);

	VkSamplerAddressMode to_vulkan_address(sampler::address);

	VkFilter to_vulkan_filter(sampler::filter);

	class vulkan_buffer_mediator;
	class vulkan_image;

	class vulkan_texture2d : public texture2d
	{
	public:

		virtual vulkan_image& image() = 0;

		static std::unique_ptr<vulkan_texture2d> make(
			const config&,
			const scoped_ptr < vulkan_buffer_mediator >&);
	};
}
