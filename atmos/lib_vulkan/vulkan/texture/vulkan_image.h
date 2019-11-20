
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_resource.h"

#include "igpu/buffer/buffer.h"

#include "framework/perf/metrics.h"
#include "framework/utility/scoped_ptr.h"

#include "glm/vec2.hpp"

namespace igpu
{
	class vulkan_buffer;
	class vulkan_barrier_manager;
	class vulkan_queues;

	class vulkan_image : public vulkan_resource
	{
	public:
		struct config
		{
			VkPhysicalDevice physical_device = nullptr;
			VkDevice device = nullptr;
			scoped_ptr< vulkan_queues > queues;
			memory_type memory = memory_type::UNDEFINED;
			VkMemoryPropertyFlagBits memory_properties;
			VkImageCreateInfo image_info = {};
			VkImageViewCreateInfo view_info = {};
			VkSamplerCreateInfo sampler_info = {};
		};

		vulkan_image( const config& cfg );

		vulkan_image( VkImageUsageFlags );

		~vulkan_image();

		const config& cfg() const;

		VkImageView vk_image_view() const;

		VkSampler vk_sampler() const;

		void reset( const config* = nullptr );

		void copy_from( vulkan_barrier_manager&, vulkan_buffer& );

		void generate_mipmaps( vulkan_barrier_manager& );

		static bool can_generate_mipmaps(
			VkPhysicalDevice,
			VkFormat format,
			VkImageTiling tiling );

		static bool validate( const config& );

		bool is_valid_layout( VkImageLayout ) const override;

	private:
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

		static std::unique_ptr< vulkan_image > make( const config& cfg );

		config _cfg;

		struct
		{
			VkImage image = nullptr;
			VkDeviceMemory device_memory = nullptr;
			VkImageView image_view = nullptr;
			VkSampler sampler = nullptr;

		} _allocation;

		vulkan_resource::state _resource_state;

		perf::metric _gpu_mem_metric;

		vulkan_image( const vulkan_image& ) = delete;
		vulkan_image& operator=( const vulkan_image& ) = delete;
	};
}
