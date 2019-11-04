
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/texture/texture2d.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	VkFormat to_vulkan_format( texture_format );

	class vulkan_synchronization;
	class vulkan_image;

	class vulkan_texture2d : public texture2d
	{
	public:
		struct config : texture2d::config
		{
			struct vulkan
			{
				VkPhysicalDevice physical_device = nullptr;
				VkDevice device = nullptr;
			};

			vulkan vk;
		};
		virtual vulkan_image& gpu_resource() = 0;

		virtual const vulkan_image& gpu_resource() const = 0;

		static std::unique_ptr< vulkan_texture2d > make(
			const config&,
			const scoped_ptr< vulkan_synchronization >& );
	};
}
