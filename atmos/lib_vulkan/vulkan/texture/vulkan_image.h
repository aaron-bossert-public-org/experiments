
#pragma once

#include <framework/perf/metrics.h>
#include <igpu/utility/scoped_ptr.h>

#include <vulkan/defines/vulkan_includes.h>

#include <glm/vec2.hpp>

namespace igpu
{
	class vulkan_fence;
	class vulkan_queue;

	class vulkan_image
	{
	public:

		struct config
		{
			VkPhysicalDevice physical_device = nullptr;
			VkDevice device = nullptr;
			VkMemoryPropertyFlagBits memory_properties;
			VkImageCreateInfo image_info = {};
			VkImageViewCreateInfo view_info = {};
			VkSamplerCreateInfo sampler_info = {};
		};

		struct ownership
		{
			VkImageLayout layout;
			VkAccessFlags access;
			VkPipelineStageFlags stage;
			VkDependencyFlags dependency;
			scoped_ptr < vulkan_queue > queue;
		};

		vulkan_image(
			const config& cfg);

		const config& cfg() const;

		const ownership& owner() const;

		const scoped_ptr < vulkan_fence >& fence() const;

		void owner(const ownership&);

		VkImage get() const;

		VkDeviceMemory device_memory() const;

		VkImageView image_view() const;

		void fence(const scoped_ptr < vulkan_fence >&);

		VkDescriptorImageInfo create_descriptor_info() const;

		~vulkan_image();

		static bool validate(const config&);

		static std::unique_ptr<vulkan_image> make(const config& cfg);

	private:

		const config _cfg;
		VkDevice _device = nullptr;
		VkImage _image = nullptr;
		VkMemoryAllocateInfo _alloc_info;
		VkDeviceMemory _device_memory = nullptr;
		VkImageView _image_view = nullptr;
		VkSampler _sampler = nullptr;

		perf::metric _gpu_mem_metric;
		scoped_ptr < vulkan_fence > _fence;

		ownership _owner = {};

		vulkan_image(const vulkan_image&) = delete;
		vulkan_image& operator= (const vulkan_image&) = delete;
	};
}
