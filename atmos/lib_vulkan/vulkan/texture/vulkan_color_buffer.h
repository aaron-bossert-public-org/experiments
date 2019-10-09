
#pragma once

#include <framework/perf/metrics.h>
#include <igpu/texture/color_buffer.h>

namespace igpu
{
	class vulkan_color_buffer : public color_buffer
	{
	public:

		static std::unique_ptr<vulkan_color_buffer> make(const config&);

		~vulkan_color_buffer();

	protected:

		vulkan_color_buffer(const config&);

	private:

		VkImage _color_image;
		VkDeviceMemory _color_image_memory;
		VkImageView _color_image_view;

		perf::metric _gpu_mem_metric;
	};
}
