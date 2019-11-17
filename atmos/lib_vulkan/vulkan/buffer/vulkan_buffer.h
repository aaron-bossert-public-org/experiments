
#pragma once


#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_resource.h"

#include "igpu/buffer/buffer.h"

#include "framework/perf/metrics.h"
#include "framework/utility/buffer_view.h"
#include "framework/utility/scoped_ptr.h"


namespace igpu
{
	class vulkan_synchronization;

	class vulkan_buffer
		: public buffer
		, public vulkan_resource
	{
	public:
		struct config
		{
			struct vulkan
			{
				VkDevice device = nullptr;
				const VkPhysicalDeviceProperties* device_properties;
				scoped_ptr< vulkan_synchronization > synchronization;
				VmaMemoryUsage vma_usage;
				VkBufferUsageFlags usage;
				VmaAllocationCreateFlagBits vma_flags = {};
				VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
			};

			memory_type memory = memory_type::WRITE_COMBINED;
			vulkan vk;
		};

		vulkan_buffer( const config& );

		~vulkan_buffer();

		const config& cfg() const;

		VkBuffer vk_buffer() const;

		void map( buffer_view_base* ) override;

		void unmap() override;

		const buffer_view< char >& mapped_view() const;

		void reset( size_t = 0 );

		void copy_from( vulkan_barrier_manager&, vulkan_buffer& );

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

		const config _cfg;

		struct
		{
			size_t memory_size = 0;
			buffer_view< char > mapped_view = {};
			VkBuffer buffer = nullptr;
			VmaAllocation vma_allocation = nullptr;
		} _allocation;

		vulkan_resource::state _resource_state;

		perf::metric _mem_metric;
	};
}
