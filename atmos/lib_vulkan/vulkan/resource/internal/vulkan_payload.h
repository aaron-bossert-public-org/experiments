
#pragma once

// Vulkan implementation includes - begin
#include <vulkan/defines/vulkan_includes.h>
// Vulkan implementation includes - end

namespace igpu
{
	class vulkan_payload
	{
	public:
		
		struct config
		{
			VmaAllocationCreateFlags vma_flags;
			VmaMemoryUsage vma_usage;
			VkBufferUsageFlagBits usage;
			VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
		};

		vulkan_payload(const config&);

		const config& cfg() const;
		
		VkBuffer buffer() const;
		VmaAllocation allocation() const;
		VkDeviceSize size() const;
		VkAccessFlagBits access() const;
		VkShaderStageFlagBits stage() const;
		uint32_t batch_index() const;
		uint32_t queue_index() const;

		void buffer(VkBuffer);
		void allocation(VmaAllocation);
		void size(VkDeviceSize);
		void access(VkAccessFlagBits);
		void stage(VkShaderStageFlagBits);
		void batch_index(uint32_t);
		void queue_index(uint32_t);

	private:

		const config _cfg;

		VkBuffer _buffer = nullptr;
		VmaAllocation _allocation = nullptr;
		VkDeviceSize _size = 0;
		VkAccessFlagBits _access = (VkAccessFlagBits)0;
		VkShaderStageFlagBits _stage = (VkShaderStageFlagBits)0;
		uint32_t _batch_index = ~0U;
		uint32_t _queue_index = ~0U;
	};
}
