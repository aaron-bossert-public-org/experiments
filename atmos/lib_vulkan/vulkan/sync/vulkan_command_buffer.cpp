
#include "vulkan/sync/vulkan_command_buffer.h"

#include "vulkan/sync/vulkan_fence.h"

using namespace igpu;


VkCommandBuffer vulkan_command_buffer::get()
{
	return _command_buffer;
}

const std::shared_ptr< vulkan_fence >& vulkan_command_buffer::fence() const
{
	return _fence;
}

vulkan_command_buffer::vulkan_command_buffer( const config& cfg )
	: _cfg( cfg )
	, _command_buffer( nullptr )
{
	if ( !_cfg.device )
	{
		LOG_CRITICAL( "device is null" );
	}
	else if ( !_cfg.command_pool )
	{
		LOG_CRITICAL( "command_pool is null" );
	}
	else
	{
		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = _cfg.command_pool;
		alloc_info.commandBufferCount = 1;

		vkAllocateCommandBuffers( _cfg.device, &alloc_info, &_command_buffer );
	}
}

vulkan_command_buffer::vulkan_command_buffer(
	const config& cfg,
	const std::shared_ptr< vulkan_fence >& fence )
	: vulkan_command_buffer( cfg )
{
	ASSERT_CONTEXT( (bool)fence );
	_fence = fence;
}

vulkan_command_buffer::~vulkan_command_buffer()
{
	if ( _fence )
	{
		_fence->wait();
	}

	if ( _command_buffer )
	{
		vkFreeCommandBuffers(
			_cfg.device,
			_cfg.command_pool,
			1,
			&_command_buffer );
	}
}
