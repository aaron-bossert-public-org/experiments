
#pragma once


#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_resource.h"

#include "igpu/buffer/buffer.h"

#include "framework/perf/metrics.h"
#include "framework/utility/buffer_view.h"
#include "framework/utility/scoped_ptr.h"


namespace igpu
{
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
				VmaAllocator vma = nullptr;
				VmaMemoryUsage vma_usage;
				VkBufferUsageFlagBits usage;
				VmaAllocationCreateFlagBits vma_flags = {};
				VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
			};

			memory_type memory = memory_type::WRITE_COMBINED;
			vulkan vk;
		};

		vulkan_buffer( const config& );

		const config& cfg() const;

		VkBuffer vk_buffer() const;

		void map( buffer_view_base* ) override;

		void unmap() override;

		void reserve( size_t byte_capacity );

		void release();

		const buffer_view< char >& mapped_view() const;

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

		~vulkan_buffer();

	private:
		const config _cfg;

		buffer_view< char > _mapped_view = {};
		VkBuffer _buffer = nullptr;
		VmaAllocation _vma_allocation = nullptr;
		vulkan_resource::state _resource_state;

		perf::metric _mem_metric;
	};
}
