
#pragma once


#include <igpu/texture/texture2d.h>
#include <framework/perf/metrics.h>
#include <glm/vec2.hpp>

namespace igpu
{
	class vulkan_texture2d : public texture2d
	{
	public:

		unsigned gl_handle() const;

		static std::unique_ptr<vulkan_texture2d> make(const texel_buffer2d&);

		virtual ~vulkan_texture2d();

		vulkan_texture2d(
			const texel_buffer2d::config&,
			size_t size,
			unsigned gl_handle);

	private:

		uint32_t _mip_levels;
		VkImage _texture_image;
		VkDeviceMemory _texture_image_memory;
		VkImageView _texture_image_view;
		VkSampler _texture_sampler;

		perf::metric _gpu_mem_metric;
	};
}
