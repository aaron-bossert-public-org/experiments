
#pragma once

#include <vulkan/texture/vulkan_image.h>

namespace igpu
{
	template <typename T>
	class vulkan_image_target_t : public T
	{
	public:

		const vulkan_image::config& image_cfg() const
		{
			return _image.cfg();
		}

		VkImage image() const
		{
			return _image.image();
		}

		VkDeviceMemory device_memory() const
		{
			return _image.device_memory();
		}

		VkImageView image_view() const
		{
			return _image.image_view();
		}

	protected:

		vulkan_image_target_t(
			const vulkan_image::config& cfg)
			: _image(cfg)
		{
		}

	private:
		vulkan_image _image;
	};
}
