
#include <vulkan/buffer/vulkan_buffer.h>
#include <vulkan/buffer/vulkan_fence.h>
#include <vulkan/buffer/vulkan_queue.h>

using namespace igpu;

namespace
{
	std::string to_string(VkBufferUsageFlagBits bits)
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

vulkan_buffer::vulkan_buffer(const config& cfg)
	: _cfg(cfg)
	, _mem_metric(to_category(cfg.vma_usage), ::to_string(cfg.vk_usage))
{
}

vulkan_buffer::~vulkan_buffer()
{
	release();
}

const vulkan_buffer::config& vulkan_buffer::cfg() const
{
	return _cfg;
}

void vulkan_buffer::map(size_t byte_size, buffer_view_base* out_buffer_view)
{
	if (_fence)
	{
		_fence->wait();
		_fence = nullptr;
	}

	if (_mapped_view.data())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else
	{
		reserve(byte_size);

		if (!_buffer)
		{
			LOG_CRITICAL("failed to create staging buffer");

			_mapped_view =
				*out_buffer_view =
					buffer_view_base(
						0,
						nullptr,
						out_buffer_view->stride());
		}
		else
		{
			void* mapped;
			vmaMapMemory(_cfg.vma, _vma_allocation, &mapped);
			
			_mapped_view = buffer_view<char>(
				_mapped_view.size(),
				(char*)mapped);

			size_t stride = out_buffer_view->stride();
			*out_buffer_view = buffer_view_base(
				byte_size / stride,
				mapped,
				stride);
		}
	}
}

void vulkan_buffer::unmap()
{
	if (!_mapped_view.data())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else
	{
		vmaUnmapMemory(_cfg.vma, _vma_allocation);
		_mapped_view = buffer_view_base(
			_mapped_view.size(),
			nullptr,
			_mapped_view.stride());
	}
}

void vulkan_buffer::reserve(size_t byte_size)
{
	if (_mapped_view.data())
	{
		LOG_CRITICAL("cannot reserve while mapped");
	}
	else
	{
		if (_mapped_view.size() < byte_size)
		{
			release();
		}

		if (!_buffer)
		{

			VmaAllocationCreateInfo vma_info = {};
			vma_info.usage = _cfg.vma_usage;
			vma_info.flags = _cfg.vma_flags;

			VkBufferCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			info.size = byte_size;
			info.usage = _cfg.vk_usage;
			info.sharingMode = _cfg.sharing_mode;

			vmaCreateBuffer(_cfg.vma, &info, &vma_info, &_buffer, &_vma_allocation, nullptr);
			_mapped_view = buffer_view<char>(
				byte_size,
				nullptr);

			_mem_metric.add(byte_size);
		}
	}
}
void vulkan_buffer::release()
{
	// wait for buffer to no longer be in use by gpu
	if (_fence)
	{
		_fence->wait();
		_fence = nullptr;
	}

	if (_buffer)
	{
		vmaDestroyBuffer(_cfg.vma, _buffer, _vma_allocation);
	}

	_mem_metric.reset();
	_buffer = nullptr;
	_vma_allocation = nullptr;
	_mapped_view = buffer_view<char>(
		0,
		nullptr);
}

size_t vulkan_buffer::byte_capacity() const
{
	return _mapped_view.byte_size();
}


const buffer_view<char>& vulkan_buffer::mapped_view() const
{
	return _mapped_view;
}

const vulkan_buffer::ownership& vulkan_buffer::owner() const
{
	return _owner;
}

VkBuffer vulkan_buffer::get() const
{
	return _buffer;
}

const scoped_ptr < vulkan_fence >& vulkan_buffer::fence() const
{
	return _fence;
}

void vulkan_buffer::owner(const ownership& owner)
{
	_owner = owner;
}

void vulkan_buffer::fence(const scoped_ptr < vulkan_fence >& fence)
{
	_fence = fence;
}
