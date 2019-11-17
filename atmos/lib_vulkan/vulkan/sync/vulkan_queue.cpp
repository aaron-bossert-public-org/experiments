
#include "vulkan/sync/vulkan_queue.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_fence.h"

#include "framework/logging/log.h"

using namespace igpu;

std::unique_ptr< vulkan_queue > vulkan_queue::make( const config& cfg )
{
	VkQueue vk_queue = nullptr;
	vkGetDeviceQueue( cfg.device, cfg.family_index, cfg.index, &vk_queue );

	if ( !vk_queue )
	{
		LOG_CRITICAL( "queue is null" );
	}
	else
	{
		VkCommandPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.queueFamilyIndex = cfg.family_index;
		pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

		VkCommandPool command_pool = nullptr;
		vkCreateCommandPool( cfg.device, &pool_info, nullptr, &command_pool );
		if ( !command_pool )
		{
			LOG_CRITICAL( "command_pool is null" );
		}
		else
		{
			return std::unique_ptr< vulkan_queue >(
				new vulkan_queue( cfg, vk_queue, command_pool ) );
		}
	}

	return nullptr;
}

const vulkan_queue::config& vulkan_queue::cfg() const
{
	return _cfg;
}
void vulkan_queue::one_time_command( const command_builder_t& builder )
{
	one_time_command( 0, nullptr, nullptr, builder, 0, nullptr );
}

void vulkan_queue::one_time_command(
	uint32_t wait_count,
	const VkSemaphore* p_wait_semaphores,
	const VkPipelineStageFlags* p_wait_stages,
	const command_builder_t& builder,
	uint32_t signal_count,
	const VkSemaphore* p_signal_semaphores,
	vulkan_fence* fence )
{
	vulkan_command_buffer command_buffer( {

		_cfg.device,
		_cfg.abandon_manager,
		_command_pool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	} );

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VkCommandBuffer vk_cmds = command_buffer.vk_cmds();

	vkBeginCommandBuffer( vk_cmds, &begin_info );
	builder( vk_cmds );
	vkEndCommandBuffer( vk_cmds );

	VkSubmitInfo info = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		wait_count,
		p_wait_semaphores,
		p_wait_stages,
		1,
		&vk_cmds,
		signal_count,
		p_signal_semaphores,
	};

	VkFence vk_fence = fence ? fence->vk_fence() : nullptr;
	vkQueueSubmit( _vk_queue, 1, &info, vk_fence );
}

VkQueue vulkan_queue::vk_queue() const
{
	return _vk_queue;
}

vulkan_queue::~vulkan_queue()
{
	vkQueueWaitIdle( _vk_queue );
	vkDestroyCommandPool( _cfg.device, _command_pool, nullptr );
}

vulkan_queue::vulkan_queue(
	const config& cfg,
	VkQueue vk_queue,
	VkCommandPool command_pool )
	: _cfg( cfg )
	, _vk_queue( vk_queue )
	, _command_pool( command_pool )
{}
