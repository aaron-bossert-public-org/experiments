
#include "vulkan/sync/vulkan_command_buffer.h"

#include "vulkan/context/vulkan_abandon_manager.h"
#include "vulkan/sync/vulkan_queue.h"

using namespace igpu;

VkCommandBuffer vulkan_command_buffer::vk_cmds() const
{
	return _command_buffer;
}

vulkan_command_buffer::vulkan_command_buffer( const config& cfg )
	: _cfg( cfg )
	, _command_buffer( nullptr )
{
	if ( !_cfg.command_pool )
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

		vkAllocateCommandBuffers(
			_cfg.queue->cfg().device,
			&alloc_info,
			&_command_buffer );
	}
}

vulkan_command_buffer::~vulkan_command_buffer()
{
	if ( _command_buffer )
	{
		abandon(
			_cfg.queue,
			_cfg.queue->cfg().device,
			_cfg.command_pool,
			_command_buffer );
	}
}
