
#pragma once

#include <framework/perf/metrics.h>
#include <vulkan/defines/vulkan_includes.h>

#include <glm/vec2.hpp>

namespace igpu
{
	class vulkan_image_buffer
	{
	public:

		struct config
		{
			VkPhysicalDevice physical_device = nullptr;
			VkDevice device = nullptr;
			VkMemoryPropertyFlagBits memory_properties;
			VkImageCreateInfo info = {};
			VkImageViewCreateInfo view_info = {};
		};

		vulkan_image_buffer(
			const config& cfg);

		const config& cfg() const;

		VkImage image() const;

		VkDeviceMemory device_memory() const;

		VkImageView image_view() const;

		~vulkan_image_buffer();

		static bool validate(const config&);

	private:

		const config _cfg;
		VkDevice _device = nullptr;
		VkImage _image = nullptr;
		VkMemoryAllocateInfo _alloc_info;
		VkDeviceMemory _device_memory = nullptr;
		VkImageView _image_view = nullptr;
		perf::metric _gpu_mem_metric;

		vulkan_image_buffer(const vulkan_image_buffer&) = delete;
		vulkan_image_buffer& operator= (const vulkan_image_buffer&) = delete;
	};

	template <typename T>
	class vulkan_image_buffer_t : public T
	{
	public:

		const vulkan_image_buffer::config& image_cfg() const
		{
			return _image_buffer.cfg();
		}

		VkImage image() const
		{
			return _image_buffer.image();
		}

		VkDeviceMemory device_memory() const
		{
			return _image_buffer.device_memory();
		}

		VkImageView image_view() const
		{
			return _image_buffer.image_view();
		}

	protected:

		vulkan_image_buffer_t(
			const vulkan_image_buffer::config& cfg)
			: _image_buffer(cfg)
			, _res(cfg.info.extent.width, cfg.info.extent.height)
		{
		}

	private:

		glm::ivec2 _res;
		vulkan_image_buffer _image_buffer;
	};
}
