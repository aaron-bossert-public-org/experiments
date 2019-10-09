
#include <vulkan/resource/vulkan_resource.h>

// Vulkan implementation includes - begin
#include <vulkan/context/vulkan_command_composer.h>
#include <vulkan/defines/vulkan_includes.h>
// Vulkan implementation includes - end

#include <framework/utility/buffer_view.h>
using namespace igpu;

vulkan_resource::vulkan_resource(
	const std::string_view& metric,
	const config& cfg)
	: _cpu_mem_metric(perf::category::CPU_MEM_USAGE, metric)
	, _gpu_mem_metric(perf::category::CPU_MEM_USAGE, metric)
	, _cfg(cfg)
	, _gpu_payload(
		0,
		VMA_MEMORY_USAGE_GPU_ONLY,
		VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | cfg.vk_usage_flags))
	, _cpu_payload(
		VMA_ALLOCATION_CREATE_MAPPED_BIT,
		VMA_MEMORY_USAGE_CPU_ONLY,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
{
}

vulkan_resource::~vulkan_resource()
{
	_cfg.command_composer->destroy(&_cpu_payload);
	_cfg.command_composer->destroy(&_gpu_payload);
}

const vulkan_resource::config& vulkan_resource::cfg() const
{
	return _cfg;
}

void vulkan_resource::map(buffer_view_base& resource_view, size_t count)
{
	if (_mapped)
	{
		LOG_CONTEXT(CRITICAL, "map/unmap mismatch");
	}
	else
	{
		size_t required_capacity = resource_view.stride() * count;
		_cfg.command_composer->ensure_capacity(&_gpu_payload, required_capacity);
		
		if (_capacity < required_capacity)
		{
			
			_capacity = required_capacity;

			VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
			buffer_info.size = _capacity;
			buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | _cfg.vk_usage_flags;
			buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo vma_alloc_create_info = {};
			vma_alloc_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
			vma_alloc_create_info.flags = 0;

			vmaCreateBuffer(_cfg.vma, &buffer_info, &vma_alloc_create_info, &_buffer, &_allocation, nullptr);

		}

		// there probably needs to be an 'else { memory barrier }'  thing off the above if, lets see if a validator points this out...
		VkBufferCreateInfo staging_buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		staging_buffer_info.size = required_capacity;
		staging_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		staging_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo staging_vma_alloc_create_info = {};
		staging_vma_alloc_create_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		staging_vma_alloc_create_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VmaAllocationInfo staging_vma_alloc_info = {};
		vmaCreateBuffer(_cfg.vma, &staging_buffer_info, &staging_vma_alloc_create_info, &_staging_buffer, &_staging_allocation, &staging_vma_alloc_info);

		_mapped = staging_vma_alloc_info.pMappedData;
		_mapped_size = count * resource_view.stride();
		_mapped_offset = 0;

		_cpu_mem_metric.reset();
		_gpu_mem_metric.reset();

		_cpu_mem_metric.add(float(_gpu_payload.size()));
		_gpu_mem_metric.add(float(_gpu_payload.size()));

		resource_view = buffer_view_base(
			count,
			_mapped,
			resource_view.stride());
	}
}

void vulkan_resource::map_subrange(buffer_view_base& resource_view, size_t offset, size_t count)
{
	if (_mapped)
	{
		LOG_CONTEXT(CRITICAL, "map/unmap mismatch");
	}
	else if (_capacity < resource_view.stride()*(offset + count))
	{
		LOG_CONTEXT(
			CRITICAL,
			"request requires %d bytes but only %d bytes is mapped",
			buffer_view.stride()*(offset + count),
			_capacity);
	}
	else
	{
		VkBufferCreateInfo staging_buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		staging_buffer_info.size = buffer_view.stride() * count;
		staging_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		staging_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo staging_vma_alloc_create_info = {};
		staging_vma_alloc_create_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		staging_vma_alloc_create_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VmaAllocationInfo staging_vma_alloc_info = {};
		vmaCreateBuffer(_cfg.vma, &staging_buffer_info, &staging_vma_alloc_create_info, &_staging_buffer, &_staging_allocation, &staging_vma_alloc_info);
		
		_mapped = staging_vma_alloc_info.pMappedData;
		_mapped_size = count * resource_view.stride();
		_mapped_offset = offset * resource_view.stride();

		resource_view = buffer_view_base(
			count,
			_mapped,
			resource_view.stride());
	}
}

void vulkan_resource::unmap(buffer_view_base& resource_view)
{
	if (resource_view.data() != _mapped)
	{
		LOG_CONTEXT(WARNING,
			"buffer to unmap:%p does not match internal record of mapped address:%p",
			resource_view.data(),
			_mapped);
	}
	else
	{
		unmap();

		resource_view = buffer_view_base(
			resource_view.size(),
			nullptr,
			resource_view.stride());
	}
}

void vulkan_resource::unmap()
{
	if (!_mapped)
	{
		LOG_CONTEXT(CRITICAL, "data is not mapped");
	}
	else if(!_staging_buffer)
	{
		LOG_CONTEXT(CRITICAL, "staging buffer is null");
	}
	else if (!_staging_allocation)
	{
		LOG_CONTEXT(CRITICAL, "staging allocation is null");
	}
	else
	{
		VkBufferCopy region = {};
		region.srcOffset = 0;
		region.dstOffset = _mapped_offset;
		region.size = _mapped_size;
		vkCmdCopyBuffer(_cfg.copier_command_buffer, _staging_buffer, _buffer, 1, &region);

		_cfg.destruction_tracker->defer_destroy(_staging_buffer, _staging_allocation);
		_staging_buffer = nullptr;
		_staging_allocation = nullptr;

		_mapped = nullptr;
		_mapped_size = 0;
		_mapped_offset = 0;
	}
}

void vulkan_resource::release()
{
	if (_mapped)
	{
		LOG_CONTEXT(CRITICAL, "cannot release while memory is mapped");
	}
	else
	{
		if (_buffer)
		{
			_cfg.destruction_tracker->defer_destroy(_buffer, _allocation);
		}
	
		_capacity = 0;
	}
}

size_t vulkan_resource::capacity() const
{
	return _capacity;
}

size_t vulkan_resource::count() const
{
	return _mapped.size();
}

size_t vulkan_resource::stride() const
{
	return _mapped.stride();
}

resource_usage vulkan_resource::usage() const
{
	return _usage;
}
