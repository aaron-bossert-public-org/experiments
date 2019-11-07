
#pragma once


#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_gpu_object.h"

#include "igpu/buffer/buffer.h"

#include "framework/perf/metrics.h"
#include "framework/utility/buffer_view.h"
#include "framework/utility/scoped_ptr.h"


namespace igpu
{
	class vulkan_queue;

	class vulkan_buffer
		: public buffer
		, public vulkan_gpu_object
	{
	public:
		struct config
		{
			struct vulkan
			{
				VmaAllocator vma = nullptr;
				VmaMemoryUsage vma_usage;
				VkBufferUsageFlagBits usage;
				VmaAllocationCreateFlagBits vma_flags = {};
				VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
			};

			memory_type memory = memory_type::WRITE_COMBINED;
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

		const config& cfg() const;

		void map( buffer_view_base* ) override;

		void unmap() override;

		void reserve( size_t byte_capacity );

		void release();

		const buffer_view< char >& mapped_view() const;

		const ownership& owner() const;

		VkBuffer get() const;

		void owner( const ownership& );

		vulkan_gpu_object::state& object_state() override;

		~vulkan_buffer();

	private:
		const config _cfg;

		buffer_view< char > _mapped_view = {};
		VkBuffer _buffer = nullptr;
		VmaAllocation _vma_allocation = nullptr;
		vulkan_gpu_object::state _object_state;

		ownership _owner = {};

		perf::metric _mem_metric;
	};
}
