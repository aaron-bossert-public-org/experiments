
#pragma once


#include <framework/perf/metrics.h>

// Vulkan implementation includes - begin
#include <vulkan/defines/vulkan_includes.h>
// Vulkan implementation includes - end

namespace igpu
{
	class vulkan_fence;
	class vulkan_queue;
	class vulkan_semaphore;

	class vulkan_buffer
	{
	public:
		
		struct config
		{
			VmaAllocator vma = nullptr;
			VmaAllocationCreateFlags vma_flags;
			VmaMemoryUsage vma_usage;
			VkBufferUsageFlagBits usage;
			VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
			VkDeviceSize size = 0;
		};

		struct ownership
		{
			VkAccessFlags access = 0;
			VkPipelineStageFlags stage = 0;
			VkDependencyFlags dependency = 0;
			std::shared_ptr < vulkan_queue > queue;
		};

		const config& cfg() const;

		const ownership& owner() const;

		VkBuffer get() const;

		const std::shared_ptr < vulkan_fence >& fence() const;

		void owner(const ownership&);
		
		void fence(const std::shared_ptr < vulkan_fence >&);

		void* map();

		void unmap();
		
		static std::unique_ptr < vulkan_buffer > make(const config&);

		~vulkan_buffer();
		
	private:

		vulkan_buffer(const config&, VkBuffer, VmaAllocation);

	private:

		const config _cfg;

		VkBuffer _buffer = nullptr;
		VmaAllocation _vma_allocation = nullptr;
		std::shared_ptr < vulkan_fence > _fence;
		
		ownership _owner;
		
		perf::metric _mem_metric;
	};
}
