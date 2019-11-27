
#include "vulkan/sync/vulkan_queue.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/manager/vulkan_abandon_manager.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_command_pool.h"
#include "vulkan/sync/vulkan_fence.h"

#include "framework/logging/log.h"

#include <array>

using namespace igpu;

namespace
{
	void submit(
		VkQueue vk_queue,
		uint32_t wait_count,
		const VkSemaphore* p_wait_semaphores,
		const VkPipelineStageFlags* p_wait_stages,
		uint32_t command_buffer_count,
		const vulkan_command_buffer* command_buffers,
		VkCommandBuffer* command_buffers_backing,
		uint32_t signal_count,
		const VkSemaphore* p_signal_semaphores,
		VkFence fence )
	{
		// copy vulkan command buffer vk pointers to backing
		VkCommandBuffer* at = command_buffers_backing;
		while ( at < command_buffers_backing + command_buffer_count )
		{
			*at = command_buffers->vk_cmds();
			++at;
			++command_buffers;
		}

		VkSubmitInfo info = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			wait_count,
			p_wait_semaphores,
			p_wait_stages,
			command_buffer_count,
			command_buffers_backing,
			signal_count,
			p_signal_semaphores,
		};

		vkQueueSubmit( vk_queue, 1, &info, fence );
	}
}

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

vulkan_abandon_manager& vulkan_queue::abandon_manager() const
{
	return *_abandon_manager;
}

void vulkan_queue::trigger_abandon()
{
	_trigger_abandon = true;
}

void vulkan_queue::submit_command( const vulkan_command_buffer& command_buffer )
{
	submit_commands( 0, nullptr, nullptr, 0, &command_buffer, 0, nullptr );
}

void vulkan_queue::submit_commands(
	uint32_t wait_count,
	const VkSemaphore* p_wait_semaphores,
	const VkPipelineStageFlags* p_wait_stages,
	uint32_t command_buffer_count,
	const vulkan_command_buffer* command_buffers,
	uint32_t signal_count,
	const VkSemaphore* p_signal_semaphores )
{
	// invoke is_ready to flush pending fences, just reducing code f
	flush_pending_fences();

	++_submit_index;

	if ( _recycled_fences.size() )
	{
		_pending_fences.push( std::move( _recycled_fences.back() ) );
		_pending_fences.back()->re_submit( _submit_index );
		_recycled_fences.pop_back();
	}
	else
	{
		_pending_fences.push( vulkan_fence::make( { this }, _submit_index ) );
	}

	VkFence vk_fence = _pending_fences.back()->vk_fence();

	// avoid dynamic allocation if there are fewer than 16 command buffers
	if ( command_buffer_count <= 16 )
	{
		std::array< VkCommandBuffer, 16 > backing;
		submit(
			_vk_queue,
			wait_count,
			p_wait_semaphores,
			p_wait_stages,
			command_buffer_count,
			command_buffers,
			backing.data(),
			signal_count,
			p_signal_semaphores,
			vk_fence );
	}
	else
	{
		std::vector< VkCommandBuffer > backing( command_buffer_count );
		submit(
			_vk_queue,
			wait_count,
			p_wait_semaphores,
			p_wait_stages,
			command_buffer_count,
			command_buffers,
			backing.data(),
			signal_count,
			p_signal_semaphores,
			vk_fence );
	}

	if ( _trigger_abandon )
	{
		_trigger_abandon = false;
		_abandon_manager->trigger_abandon();
	}
}

ptrdiff_t vulkan_queue::submit_index() const
{
	return _submit_index;
}

bool vulkan_queue::is_ready( ptrdiff_t submit_index )
{
	if ( 0 > ( _submit_index - submit_index ) )
	{
		LOG_CRITICAL( "sumission %d has not occurred yet ", submit_index );
		return false;
	}

	flush_pending_fences();
	if ( _pending_fences.empty() )
	{
		return true;
	}

	ptrdiff_t cmp = _pending_fences.front()->submit_index() - submit_index;

	return _pending_fences.empty() || 0 < cmp;
}

void vulkan_queue::wait( ptrdiff_t submit_index )
{
	if ( 0 > ( _submit_index - submit_index ) )
	{
		LOG_CRITICAL( "sumission %d has not occurred yet ", submit_index );
	}
	else
	{
		while ( !_pending_fences.empty() )
		{
			ptrdiff_t cmp =
				_pending_fences.front()->submit_index() - submit_index;

			if ( 0 >= cmp )
			{
				if ( 0 == cmp )
				{
					_pending_fences.front()->wait();
				}

				_recycled_fences.push_back(
					std::move( _pending_fences.front() ) );
				_pending_fences.pop();
			}

			if ( 0 <= cmp )
			{
				break;
			}
		}
	}
}

VkResult vulkan_queue::submit_present( const VkPresentInfoKHR& present_info )
{
	return vkQueuePresentKHR( _vk_queue, &present_info );
}

void vulkan_queue::flush_pending_fences()
{
	while ( !_pending_fences.empty() && _pending_fences.front()->is_ready() )
	{
		_recycled_fences.push_back( std::move( _pending_fences.front() ) );
		_pending_fences.pop();
	}
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
	, _abandon_manager( vulkan_abandon_manager::make( { this } ) )
{}
