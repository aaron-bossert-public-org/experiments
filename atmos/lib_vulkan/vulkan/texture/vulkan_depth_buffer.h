
#pragma once

#include <igpu/texture/depth_target.h>
#include <vulkan/texture/vulkan_image_target_t.h>

namespace igpu
{
	VkFormat to_vulkan_format(depth_format format);

	class vulkan_depth_buffer : public vulkan_image_target_t < depth_target >
	{

	public:

		struct config : depth_target::config
		{
			VkPhysicalDevice physical_device = nullptr;
			VkDevice device = nullptr;
			VkSampleCountFlagBits sample_count;
			VkSharingMode sharing_mode;
		};

		const config& cfg() const override;

		static std::unique_ptr<vulkan_depth_buffer> make(const config&);

	private:

		vulkan_depth_buffer(
			const config&,
			const vulkan_image::config&);

	private:

		const config _cfg;
	};
}

