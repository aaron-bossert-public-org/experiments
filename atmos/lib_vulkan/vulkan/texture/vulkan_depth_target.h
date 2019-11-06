
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
			VkSampleCountFlagBits sample_count;
			VkSharingMode sharing_mode;
		};

		virtual vulkan_image& gpu_object() = 0;

		virtual const vulkan_image& gpu_object() const = 0;
	};
}
