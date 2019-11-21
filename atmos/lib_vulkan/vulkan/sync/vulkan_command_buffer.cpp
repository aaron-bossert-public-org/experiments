
#include "vulkan/sync/vulkan_command_buffer.h"

#include "vulkan/context/vulkan_abandon_manager.h"
#include "vulkan/sync/vulkan_command_pool.h"
#include "vulkan/sync/vulkan_queue.h"

using namespace igpu;

VkCommandBuffer vulkan_command_buffer::vk_cmds() const
{
	return _vk_cmds;
}

void vulkan_command_buffer::vk_pool( VkCommandPool vk_pool )
{
	_vk_pool = vk_pool;
}

vulkan_command_buffer::vulkan_command_buffer( const config& cfg )
	: _cfg( cfg )
{
	if ( !_cfg.command_pool )
	{
		LOG_CRITICAL( "command_pool is null" );
	}
	else
	{
		_cfg.command_pool->get_vk_pool( this );

		ASSERT_CONTEXT( (bool)_vk_pool );

		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = _cfg.level;
		alloc_info.commandPool = _vk_pool;
		alloc_info.commandBufferCount = 1;

		vkAllocateCommandBuffers(
			_cfg.command_pool->cfg().queue->cfg().device,
			&alloc_info,
			&_vk_cmds );
	}
}

vulkan_command_buffer::~vulkan_command_buffer()
{
	if ( _vk_cmds )
	{
		auto queue = _cfg.command_pool->cfg().queue;
		abandon( queue, queue->cfg().device, _vk_pool, _vk_cmds );
	}
}
