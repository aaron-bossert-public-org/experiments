
#include "vulkan/buffer/vulkan_staged_buffer.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_synchronization.h"

#include "framework/utility/buffer_view.h"
using namespace igpu;

namespace
{
	VmaMemoryUsage cpu_vma_usage( memory_type memory )
	{
		switch ( memory )
		{
		case memory_type::WRITE_COMBINED:
			return VMA_MEMORY_USAGE_CPU_ONLY;
		case memory_type::PRESERVED:
			return VMA_MEMORY_USAGE_CPU_ONLY;
		}

		return VMA_MEMORY_USAGE_UNKNOWN;
	}

	VkBufferUsageFlags cpu_usage( memory_type memory )
	{
		switch ( memory )
		{
		case memory_type::WRITE_COMBINED:
			return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		case memory_type::PRESERVED:
			return VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
				VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}

		return (VkBufferUsageFlagBits)0;
	}

	VmaMemoryUsage gpu_vma_usage( memory_type memory )
	{
		switch ( memory )
		{
		case memory_type::WRITE_COMBINED:
			return VMA_MEMORY_USAGE_GPU_ONLY;
		case memory_type::PRESERVED:
			return VMA_MEMORY_USAGE_GPU_ONLY;
		}

		return VMA_MEMORY_USAGE_UNKNOWN;
	}

	VkBufferUsageFlags gpu_usage( memory_type memory )
	{
		switch ( memory )
		{
		case memory_type::WRITE_COMBINED:
			return VK_BUFFER_USAGE_TRANSFER_DST_BIT |
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		case memory_type::PRESERVED:
			return VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
				VK_BUFFER_USAGE_TRANSFER_DST_BIT |
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}

		return (VkBufferUsageFlagBits)0;
	}
}

vulkan_staged_buffer::vulkan_staged_buffer( const config& cfg )
	: _cfg( cfg )
	, _staging_buffer( {
		  cfg.memory,
		  cfg.device,
		  cfg.device_properties,
		  cfg.synchronization,
		  cpu_vma_usage( cfg.memory ),
		  cpu_usage( cfg.memory ),
	  } )
	, _gpu_buffer( {
		  memory_type::WRITE_COMBINED,
		  cfg.device,
		  cfg.device_properties,
		  cfg.synchronization,
		  gpu_vma_usage( cfg.memory ),
		  gpu_usage( cfg.memory ) | cfg.vk_usage_flags,
	  } )
{}

vulkan_staged_buffer::~vulkan_staged_buffer()
{}

void vulkan_staged_buffer::map( buffer_view_base* out_buffer_view )
{
	_staging_buffer.map( out_buffer_view );
	_byte_size = out_buffer_view->byte_size();
}

void vulkan_staged_buffer::unmap()
{
	if ( !_staging_buffer.mapped_view().data() )
	{
		LOG_CRITICAL( "map/unmap mismatch" );
	}
	else
	{
		_staging_buffer.unmap();
		_gpu_buffer.copy_from( *_cfg.barrier_manager, _staging_buffer );
		_staging_buffer.reset();
	}
}

vulkan_buffer& vulkan_staged_buffer::gpu_buffer()
{
	return _gpu_buffer;
}

const vulkan_buffer& vulkan_staged_buffer::gpu_buffer() const
{
	return _gpu_buffer;
}

size_t vulkan_staged_buffer::byte_size() const
{
	return _byte_size;
}