
#include <vulkan/buffer/vulkan_buffer.h>
#include <vulkan/buffer/vulkan_fence.h>
#include <vulkan/buffer/vulkan_queue.h>

using namespace igpu;

namespace
{
	std::string stringify(VkBufferUsageFlagBits bits)
	{
		std::string str;

		if (bits & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) str += "transfer src";
		if (bits & VK_BUFFER_USAGE_TRANSFER_DST_BIT) str += "transfer dst";
		if (bits & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT) str += "uniform texel";
		if (bits & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT) str += "storage texel";
		if (bits & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) str += "uniform buffer";
		if (bits & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) str += "storage buffer";
		if (bits & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) str += "index buffer";
		if (bits & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) str += "vertex buffer";
		if (bits & VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT) str += "indirect buffer";
		if (bits & VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT) str += "transform feedback";
		if (bits & VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT) str += "transform feedback";
		if (bits & VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT) str += "conditional rendering";
		if (bits & VK_BUFFER_USAGE_RAY_TRACING_BIT_NV) str += "ray tracing";
		if (bits & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_EXT) str += "shader device";

		return str;
	}

	perf::category to_category(VmaMemoryUsage usage)
	{
		switch (usage)
		{
		case VMA_MEMORY_USAGE_GPU_ONLY:
			return perf::category::GPU_MEM_USAGE;
		case VMA_MEMORY_USAGE_CPU_ONLY:
			return perf::category::CPU_MEM_USAGE;
		case VMA_MEMORY_USAGE_CPU_TO_GPU:
			return perf::category::CPU_GPU_SHARED_MEM_USAGE;
		case VMA_MEMORY_USAGE_GPU_TO_CPU:
			return perf::category::CPU_GPU_SHARED_MEM_USAGE;
		};

		return perf::category::UNDEFINED;
	}
}

const vulkan_buffer::config& vulkan_buffer::cfg() const
{
	return _cfg;
}

const vulkan_buffer::ownership& vulkan_buffer::owner() const
{
	return _owner;
}

VkBuffer vulkan_buffer::get() const
{
	return _buffer;
}

const std::shared_ptr < vulkan_fence >& vulkan_buffer::fence() const
{
	return _fence;
}

void vulkan_buffer::owner(const ownership& owner)
{
	_owner = owner;
}

void vulkan_buffer::fence(const std::shared_ptr < vulkan_fence >& fence)
{
	_fence = fence;
}

void* vulkan_buffer::map()
{
	if (_fence)
	{
		_fence->wait();
	}

	void* mapped;
	vmaMapMemory(_cfg.vma, _vma_allocation, &mapped);
	return mapped;
}

void vulkan_buffer::unmap()
{
	vmaUnmapMemory(_cfg.vma, _vma_allocation);
}

std::unique_ptr < vulkan_buffer > vulkan_buffer::make(const config& cfg)
{
	if (0 >= cfg.size)
	{
		LOG_CRITICAL("size is zero");
	}
	else if (VMA_MEMORY_USAGE_UNKNOWN == cfg.vma_usage)
	{
		LOG_CRITICAL("vma usage is unknown");
	}
	else if (0 == cfg.usage)
	{
		LOG_CRITICAL("usage is not set");
	}
	else
	{
		VmaAllocationCreateInfo vma_info = {};
		vma_info.usage = cfg.vma_usage;
		vma_info.flags = cfg.vma_flags;

		VkBufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.size = cfg.size;
		info.usage = cfg.usage;
		info.sharingMode = cfg.sharing_mode;

		VkBuffer buffer = nullptr;
		VmaAllocation vma_allocation = nullptr;

		if (VK_SUCCESS == vmaCreateBuffer(cfg.vma, &info, &vma_info, &buffer, &vma_allocation, nullptr))
		{
			return std::unique_ptr < vulkan_buffer >(
				new vulkan_buffer(
					cfg,
					buffer,
					vma_allocation));
		}
	}

	return nullptr;
}

vulkan_buffer::~vulkan_buffer()
{
	// wait for buffer to no longer be in use by gpu
	if(_fence)
	{
		_fence->wait();
	}

	vmaDestroyBuffer(_cfg.vma, _buffer, _vma_allocation);
}

vulkan_buffer::vulkan_buffer(const config& cfg, VkBuffer buffer, VmaAllocation vma_allocation)
	: _cfg(cfg)
	, _buffer(buffer)
	, _vma_allocation(vma_allocation)
	, _mem_metric(to_category(cfg.vma_usage), stringify(cfg.usage))
{
	_mem_metric.add(cfg.size);
}

