
#pragma once

#include "framework/utility/scoped_ptr.h"
#include "framework/perf/metrics.h"

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_resource.h"

#include "glm/vec2.hpp"

namespace igpu
{
	class vulkan_queue;

	class vulkan_image : public vulkan_resource
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

		void owner(const ownership&);

		VkImage get() const;

		VkDeviceMemory device_memory() const;

		VkImageView image_view() const;

		VkDescriptorImageInfo create_descriptor_info() const;

		~vulkan_image();

		vulkan_resource::state& resource_state() override;

		static bool validate(const config&);

		static std::unique_ptr<vulkan_image> make(const config& cfg);

	private:

		const config _cfg;
		VkImage _image = nullptr;
		VkMemoryAllocateInfo _alloc_info;
		VkDeviceMemory _device_memory = nullptr;
		VkImageView _image_view = nullptr;
		VkSampler _sampler = nullptr;
		vulkan_resource::state _resource_state;

		perf::metric _gpu_mem_metric;

		ownership _owner = {};

		vulkan_image(const vulkan_image&) = delete;
		vulkan_image& operator= (const vulkan_image&) = delete;
	};
}
