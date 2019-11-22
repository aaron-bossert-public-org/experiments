
#include "vulkan/sync/vulkan_queue.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/manager/vulkan_abandon_manager.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_command_pool.h"
#include "vulkan/sync/vulkan_poset_fence.h"

#include "framework/logging/log.h"

#include <array>

using namespace igpu;

std::shared_ptr< vulkan_queue > vulkan_queue::make( const config& cfg )
{
	VkQueue vk_queue = nullptr;
	vkGetDeviceQueue( cfg.device, cfg.family_index, 0, &vk_queue );

	if ( !vk_queue )
	{
		LOG_CRITICAL( "queue is null" );
	}
	else
	{
		auto shared = std::shared_ptr< vulkan_queue >(
			new vulkan_queue( cfg, vk_queue ) );

		shared->_command_pool = vulkan_command_pool ::make( {
			shared,
			VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
		} );

		if ( shared->_command_pool )
		{
			return shared;
		}
	}

	return nullptr;
}

const vulkan_queue::config& vulkan_queue::cfg() const
{
	return _cfg;
}

scoped_ptr< vulkan_command_pool > vulkan_queue::command_pool() const
{
	return _command_pool;
}

ptrdiff_t vulkan_queue::submit_index() const
{
	return _submit_index;
}

vulkan_abandon_manager& vulkan_queue::abandon_manager() const
{
	return *_abandon_manager;
}

void vulkan_queue::trigger_abandon()
{
	_trigger_abandon = true;
}

void vulkan_queue::push_command( vulkan_command_buffer& command_buffer )
{
	vulkan_command_buffer* p_command_buffer = &command_buffer;
	push_commands( 0, nullptr, nullptr, 0, &p_command_buffer, 0, nullptr );
}

void vulkan_queue::push_commands(
	uint32_t wait_count,
	VkSemaphore* p_wait_semaphores,
	VkPipelineStageFlags* p_wait_stages,
	uint32_t command_buffer_count,
	vulkan_command_buffer** command_buffers,
	uint32_t signal_count,
	VkSemaphore* p_signal_semaphores )
{
	_pending_semaphores.insert(
		_pending_semaphores.end(),
		p_wait_semaphores,
		p_wait_semaphores + wait_count );

	_pending_wait_stage_flags.insert(
		_pending_wait_stage_flags.end(),
		p_wait_stages,
		p_wait_stages + wait_count );

	_pending_semaphores.insert(
		_pending_semaphores.end(),
		p_signal_semaphores,
		p_signal_semaphores + signal_count );

	_pending_command_buffers.reserve(
		_pending_command_buffers.size() + command_buffer_count );

	for ( size_t i = 0; i < command_buffer_count; ++i )
	{
		auto& cb = command_buffers[i];
		if ( cb->queue().get() != this )
		{
			LOG_CRITICAL( "command buffer was not created for this queue" );
		}
		else
		{
			_pending_command_buffers.push_back( cb->vk_cmds() );
		}
	}

	_pending_submit_info.push_back( {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		wait_count,
		nullptr,
		nullptr,
		command_buffer_count,
		nullptr,
		signal_count,
		nullptr,
	} );
}

void vulkan_queue::submit_pending( std::shared_ptr< vulkan_poset_fence > fence )
{
	auto at_cb = _pending_command_buffers.begin();
	auto at_sem = _pending_semaphores.begin();
	auto at_flags = _pending_wait_stage_flags.begin();
	for ( auto& submit_info : _pending_submit_info )
	{
		if ( submit_info.waitSemaphoreCount )
		{
			submit_info.pWaitSemaphores = &*at_sem;
			at_sem += submit_info.waitSemaphoreCount;
		}

		if ( submit_info.waitSemaphoreCount )
		{
			submit_info.pWaitDstStageMask = &*at_flags;
			at_flags += submit_info.waitSemaphoreCount;
		}

		if ( submit_info.commandBufferCount )
		{
			submit_info.pCommandBuffers = &*at_cb;
			at_cb += submit_info.commandBufferCount;
		}

		if ( submit_info.signalSemaphoreCount )
		{
			submit_info.pSignalSemaphores = &*at_sem;
			at_sem += submit_info.signalSemaphoreCount;
		}
	}

	if ( _trigger_abandon && !fence )
	{
		fence = vulkan_poset_fence::make( { _cfg.device } );
	}

	VkFence vk_fence = fence ? fence->vk_fence() : nullptr;

	++_submit_index;

	if ( fence )
	{
		fence->on_submit( *this );
	}

	vkQueueSubmit(
		_vk_queue,
		(uint32_t)_pending_submit_info.size(),
		_pending_submit_info.data(),
		vk_fence );

	_pending_submit_info.clear();
	_pending_command_buffers.clear();
	_pending_semaphores.clear();
	_pending_wait_stage_flags.clear();

	if ( _trigger_abandon )
	{
		_trigger_abandon = false;
		_abandon_manager->trigger_abandon( fence );
	}
}

VkResult vulkan_queue::submit_present( const VkPresentInfoKHR& present_info )
{
	return vkQueuePresentKHR( _vk_queue, &present_info );
}

vulkan_queue::~vulkan_queue()
{
	vkQueueWaitIdle( _vk_queue );

	_abandon_manager = nullptr;

	// if the scoped_ptr<vulkan_queue> is destroyed inside vulkan_queue it
	// asserts.
	_command_pool->_cfg.queue = nullptr;
	_command_pool = nullptr;
}

vulkan_queue::vulkan_queue( const config& cfg, VkQueue vk_queue )
	: _cfg( cfg )
	, _vk_queue( vk_queue )
	, _abandon_manager( vulkan_abandon_manager::make() )
{}
