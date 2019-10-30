
#pragma once


#include <framework/perf/metrics.h>
#include <framework/utility/scoped_ptr.h>

#include <igpu/buffer/buffer.h>

#include <vulkan/defines/vulkan_includes.h>


namespace igpu
{
	class vulkan_fence;
	class vulkan_queue;

	class vulkan_buffer : public buffer
	{
	public:
		
		struct config : buffer::config
		{
			struct vulkan
			{
				VmaAllocator vma = nullptr;
				VmaMemoryUsage vma_usage;
				VkBufferUsageFlagBits usage;
				VmaAllocationCreateFlagBits vma_flags = (VmaAllocationCreateFlagBits)0;
				VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
			};
			
			vulkan vk;
		};

		struct ownership
		{
			VkAccessFlags access;
			VkPipelineStageFlags stage;
			VkDependencyFlags dependency;
			scoped_ptr < vulkan_queue > queue;
		};

		vulkan_buffer(const config&);

		const config& cfg() const override;

		void map(size_t byte_size, buffer_view_base*) override;

		void unmap() override;

		size_t byte_capacity() const override;

		void reserve(size_t byte_capacity);

		void release();

		const buffer_view<char>& mapped_view() const;

		const ownership& owner() const;

		VkBuffer get() const;

		const scoped_ptr < vulkan_fence >& fence() const;

		void owner(const ownership&);
		
		void fence(const scoped_ptr < vulkan_fence >&);

		~vulkan_buffer();
		
	private:

		const config _cfg;

		buffer_view<char> _mapped_view = {};
		VkBuffer _buffer = nullptr;
		VmaAllocation _vma_allocation = nullptr;
		scoped_ptr < vulkan_fence > _fence;
		
		ownership _owner = {};
		
		perf::metric _mem_metric;
	};
}
