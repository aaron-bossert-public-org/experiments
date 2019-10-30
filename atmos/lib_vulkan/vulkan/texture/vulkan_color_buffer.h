
#pragma once

#include <igpu/texture/color_target.h>
#include <vulkan/texture/vulkan_image_target_t.h>

namespace igpu
{
	VkFormat to_vulkan_format(color_format);

	class vulkan_color_buffer : public vulkan_image_target_t < color_target >
	{

	public:
		
		struct config : color_target::config
		{
			struct vulkan
			{
				VkPhysicalDevice physical_device = nullptr;
				VkDevice device = nullptr;
				VkSampleCountFlagBits sample_count;
				VkSharingMode sharing_mode;
			};
			
			vulkan vk;
		};

		const config& cfg() const override;

		static std::unique_ptr<vulkan_color_buffer> make(const config&);

	private:

		vulkan_color_buffer(
			const config&,
			const vulkan_image::config&);

	private:

		const config _cfg;
	};
}
