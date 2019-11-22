
#include "vulkan/sync/vulkan_command_pool.h"

#include "vulkan/manager/vulkan_abandon_manager.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_queue.h"

using namespace igpu;

const vulkan_command_pool::config& vulkan_command_pool::cfg() const
{
	return _cfg;
}

void vulkan_command_pool::get_vk_pool( vulkan_command_buffer* command_buffer )
{
	command_buffer->vk_pool( _vk_pool );
}

std::unique_ptr< vulkan_command_pool > vulkan_command_pool::make(
	const config& cfg )
{
	auto queue = cfg.queue;

	if ( !queue )
	{
		LOG_CRITICAL( "queue is null" );
	}
	else
	{
		VkCommandPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.queueFamilyIndex = queue->cfg().family_index;
		pool_info.flags = cfg.flags;

		std::unique_ptr< vulkan_command_pool > unique;
		unique.reset( new vulkan_command_pool( cfg ) );

		vkCreateCommandPool(
			queue->cfg().device,
			&pool_info,
			nullptr,
			&unique->_vk_pool );
		if ( unique->_vk_pool )
		{
			return unique;
		}
	}

	return nullptr;
}

vulkan_command_pool::vulkan_command_pool( const config& cfg )
	: _cfg( cfg )
	, _device( cfg.queue->cfg().device )
{}

vulkan_command_pool::~vulkan_command_pool()
{
	if ( _vk_pool )
	{
		abandon( _cfg.queue, _device, _vk_pool );
	}
}
