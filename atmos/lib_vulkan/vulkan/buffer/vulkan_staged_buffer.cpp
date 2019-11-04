
#include "vulkan/buffer/vulkan_staged_buffer.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_synchronization.h"

#include "framework/utility/buffer_view.h"
using namespace igpu;

vulkan_staged_buffer::vulkan_staged_buffer( const config& cfg )
	: _cfg( cfg )
	, _staging_buffer( {
		  cfg.usage,
		  cfg.synchronization->vma(),
		  VMA_MEMORY_USAGE_CPU_ONLY,
		  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	  } )
	, _gpu_buffer( {
		  cfg.usage,
		  cfg.synchronization->vma(),
		  VMA_MEMORY_USAGE_GPU_ONLY,
		  VkBufferUsageFlagBits(
			  VK_BUFFER_USAGE_TRANSFER_DST_BIT | cfg.vk_usage_flags ),
	  } )
{}

vulkan_staged_buffer::~vulkan_staged_buffer()
{}

const vulkan_staged_buffer::config& vulkan_staged_buffer::cfg() const
{
	return _cfg;
}

void vulkan_staged_buffer::map( buffer_view_base* out_buffer_view )
{
	_staging_buffer.map( out_buffer_view );
	_last_mapped_bytes = out_buffer_view->byte_size();
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

		_gpu_buffer.reserve( _last_mapped_bytes );

		_cfg.synchronization->copy(
			_staging_buffer,
			_gpu_buffer,
			(uint32_t)_last_mapped_bytes );

		if ( _cfg.usage == buffer_usage::STATIC )
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
