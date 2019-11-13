
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_resource.h"

#include "framework/perf/metrics.h"
#include "framework/utility/scoped_ptr.h"

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

		vulkan_image( const config& cfg );

		vulkan_image( VkImageUsageFlags );

		void reallocate( const config& cfg );

		const config& cfg() const;

		~vulkan_image();

		vulkan_resource::state& resource_state() override;

		const vulkan_resource::state& resource_state() const override;

		void update_descriptor_set(
			VkDescriptorSet descriptor_set,
			const vulkan_parameter::config&,
			size_t array_element ) const override;

		void push_barrier(
			uint32_t target_queue_family_index,
			vulkan_barrier_manager*,
			uint32_t src_queue_family_index,
			uint32_t dst_queue_family_index,
			VkImageLayout src_layout,
			VkImageLayout dst_layout,
			const vulkan_job_scope& src_scope,
			const vulkan_job_scope& dst_scope ) const override;

		static bool validate( const config& );

		static std::unique_ptr< vulkan_image > make( const config& cfg );

	private:
		void release();

	private:
		config _cfg;
		VkImage _image = nullptr;
		VkMemoryAllocateInfo _alloc_info;
		VkDeviceMemory _device_memory = nullptr;
		VkImageView _image_view = nullptr;
		VkSampler _sampler = nullptr;
		vulkan_resource::state _resource_state;

		perf::metric _gpu_mem_metric;

		vulkan_image( const vulkan_image& ) = delete;
		vulkan_image& operator=( const vulkan_image& ) = delete;
	};
}
