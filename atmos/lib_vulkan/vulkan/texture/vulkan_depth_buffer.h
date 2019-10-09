
#pragma once

#include <igpu/texture/depth_buffer.h>
#include <framework/perf/metrics.h>

namespace igpu
{
	class vulkan_depth_buffer : public depth_buffer
	{
	public:

		static std::unique_ptr<vulkan_depth_buffer> make(const config&);

		~vulkan_depth_buffer();

	protected:

		vulkan_depth_buffer(const config&);

	private:

		VkImage _depth_image;
		VkDeviceMemory _depth_image_memory;
		VkImageView _depth_image_view;

		perf::metric _gpu_mem_metric;
	};
}
