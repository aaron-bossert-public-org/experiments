
#include "vulkan/buffer/vulkan_staged_buffer.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_synchronization.h"

#include "framework/utility/buffer_view.h"
using namespace igpu;

vulkan_staged_buffer::vulkan_staged_buffer( const config& cfg )
	: _cfg( cfg )
	, _staging_buffer( {
		  cfg.memory,
		  cfg.device,
		  cfg.device_properties,
		  cfg.synchronization->vma(),
		  VMA_MEMORY_USAGE_CPU_ONLY,
		  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	  } )
	, _gpu_buffer( {
		  memory_type::WRITE_COMBINED,
		  cfg.device,
		  cfg.device_properties,
		  cfg.synchronization->vma(),
		  VMA_MEMORY_USAGE_GPU_ONLY,
		  VkBufferUsageFlagBits(
			  cfg.vk_usage_flags | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
			  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT ),
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

		_gpu_buffer.reserve( _byte_size );

		_cfg.synchronization->copy(
			_staging_buffer,
			_gpu_buffer,
			(uint32_t)_byte_size );

		if ( _cfg.memory == memory_type::WRITE_COMBINED )
		{
			_staging_buffer.release();
		}
	}
}

void vulkan_staged_buffer::release()
{
	if ( _staging_buffer.mapped_view().data() )
	{
		LOG_CRITICAL( "cannot release while memory is mapped" );
	}
	else
	{
		_staging_buffer.release();
		_gpu_buffer.release();
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