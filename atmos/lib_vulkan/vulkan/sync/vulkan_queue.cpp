
#include "vulkan/sync/vulkan_queue.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_fence.h"

using namespace igpu;

std::unique_ptr< vulkan_queue > vulkan_queue::make( const config& cfg )
{
	VkQueue queue = nullptr;
	vkGetDeviceQueue( cfg.device, cfg.family_index, cfg.index, &queue );

	VkCommandPool command_pool;
	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = cfg.family_index;

	vkCreateCommandPool( cfg.device, &pool_info, nullptr, &command_pool );

	return std::unique_ptr< vulkan_queue >(
		new vulkan_queue( cfg, queue, command_pool ) );
}

const vulkan_queue::config& vulkan_queue::cfg() const
{
	return _cfg;
}

VkQueue vulkan_queue::get()
{
	return _queue;
}

VkCommandPool vulkan_queue::command_pool()
{
	return _command_pool;
}

std::list< vulkan_command_buffer >& vulkan_queue::pending_commands()
{
	return _pending_commands;
}

void vulkan_queue::free_completed_commands()
{
	// free front chunk of oldest command buffers
	while ( _pending_commands.size() &&
			_pending_commands.front().fence()->is_ready() )
	{
		_pending_commands.erase( _pending_commands.begin() );
	}
}

vulkan_queue::~vulkan_queue()
{
	_pending_commands.clear();

	vkDestroyCommandPool( _cfg.device, _command_pool, nullptr );
}

vulkan_queue::vulkan_queue(
	const config& cfg,
	VkQueue queue,
	VkCommandPool command_pool )
	: _cfg( cfg )
	, _queue( queue )
	, _command_pool( command_pool )
{}
