
#pragma once


#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_resource.h"

#include "igpu/buffer/buffer.h"

#include "framework/perf/metrics.h"
#include "framework/utility/scoped_ptr.h"


namespace igpu
{
	class vulkan_queue;

	class vulkan_buffer
		: public buffer
		, public vulkan_resource
	{
	public:
		struct config : buffer::config
		{
			struct vulkan
			{
				VmaAllocator vma = nullptr;
				VmaMemoryUsage vma_usage;
				VkBufferUsageFlagBits usage;
				VmaAllocationCreateFlagBits vma_flags =
					(VmaAllocationCreateFlagBits)0;
				VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
			};

			vulkan vk;
		};

		struct ownership
		{
			VkAccessFlags access;
			VkPipelineStageFlags stage;
			VkDependencyFlags dependency;
			scoped_ptr< vulkan_queue > queue;
		};

		vulkan_buffer( const config& );

		const config& cfg() const override;

		void map( buffer_view_base* ) override;

		void unmap() override;

		void reserve( size_t byte_capacity );

		void release();

		const buffer_view< char >& mapped_view() const;

		const ownership& owner() const;

		VkBuffer get() const;

		void owner( const ownership& );

		vulkan_resource::state& resource_state() override;

		~vulkan_buffer();

	private:
		const config _cfg;

		buffer_view< char > _mapped_view = {};
		VkBuffer _buffer = nullptr;
		VmaAllocation _vma_allocation = nullptr;
		vulkan_resource::state _resource_state;

		ownership _owner = {};

		perf::metric _mem_metric;
	};
}
