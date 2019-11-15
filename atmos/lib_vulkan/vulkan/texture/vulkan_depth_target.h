
#pragma once

#include "vulkan/defines/vulkan_includes.h"

namespace igpu
{
	class vulkan_image;

	class vulkan_depth_target
	{
	public:
		struct vulkan
		{
			VkPhysicalDevice physical_device = nullptr;
			VkDevice device = nullptr;
			VkFormat format = VK_FORMAT_UNDEFINED;
			VkSampleCountFlagBits sample_count = (VkSampleCountFlagBits)0;
			VkSharingMode sharing_mode;
		};

		virtual vulkan_image& gpu_object() = 0;

		virtual const vulkan_image& gpu_object() const = 0;
	};
}
