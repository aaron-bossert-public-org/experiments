
#include "vulkan/manager/vulkan_staging_manager.h"

#include "vulkan/manager/vulkan_barrier_manager.h"
#include "vulkan/manager/vulkan_managers.h"
#include "vulkan/manager/vulkan_queue_manager.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_command_pool.h"
#include "vulkan/sync/vulkan_queue.h"
#include "vulkan/sync/vulkan_resource.h"

#include "framework/logging/log.h"

using namespace igpu;

void vulkan_staging_manager::start_recording_transfers()
{
	if ( _recording_transfers )
	{
		LOG_CRITICAL(
			"push_transfers has not been called since last "
			"start_recording_transfers" );
	}
	else if ( 0 != _pending_resources.size() )
	{
		LOG_CRITICAL(
			"_pending_resources has not been cleared since last "
			"push_transfers" );
	}

	_recording_transfers = true;
}

void vulkan_staging_manager::record_transfer( vulkan_resource* resource )
{
	if ( !resource )
	{
		LOG_CRITICAL( "resource is null" );
	}
	else if ( !_recording_transfers )
	{
		LOG_CRITICAL(
			"start_recording_transfers has not been called since last "
			"push_transfers" );
	}
	else
	{
		const auto& record_ref = resource->staging_record_ref();
		if ( !record_ref.recording_manager )
		{
			resource->staging_record_ref( { this, _pending_resources.size() } );

			_pending_resources.push_back( resource );
		}
	}
}

void vulkan_staging_manager::push_pending_transfers()
{
	if ( !_recording_transfers )
	{
		LOG_CRITICAL(
			"start_recording_transfers has not been called since last "
			"push_transfers" );
	}
	else
	{
		_recording_transfers = false;


		const auto& transfer_queue = _cfg.queue_manager->cfg().transfer_queue;

		vulkan_command_buffer command_buffer( {
			transfer_queue->command_pool(),
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		} );

		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		_vk_cmds = command_buffer.vk_cmds();
		vkBeginCommandBuffer( _vk_cmds, &begin_info );
		for ( vulkan_resource* resource : _pending_resources )
		{
			resource->push_transfer();
		}
		vkEndCommandBuffer( _vk_cmds );
		_vk_cmds = nullptr;

		_cfg.barrier_manager->submit_frame_job_barriers(
			transfer_queue,
			_pending_barriers.size(),
			_pending_barriers.data() );

		for ( vulkan_resource* resource : _pending_resources )
		{
			resource->finalize_transfer();
			resource->staging_record_ref( {} );
		}
		_pending_resources.clear();

		transfer_queue->push_command( command_buffer );
	}
}

void vulkan_staging_manager::push_transfer(
	const std::initializer_list< frame_job_barrier >& job_barriers,
	const std::function< void( VkCommandBuffer ) >& builder )
{
	if ( !_vk_cmds )
	{
		LOG_CRITICAL(
			"push_transfer must be called indirectly during a call to "
			"vulkan_staging_manager::push_pending_transfers" );
	}
	else
	{
		_pending_barriers.insert(
			_pending_barriers.end(),
			job_barriers.begin(),
			job_barriers.end() );
		builder( _vk_cmds );
	}
}

std::unique_ptr< vulkan_staging_manager > vulkan_staging_manager::make(
	const config& cfg )
{
	if ( !cfg.queue_manager )
	{
		LOG_CRITICAL( "queue manager is null" );
	}
	else if ( !cfg.barrier_manager )
	{
		LOG_CRITICAL( "barrier_manager manager is null" );
	}
	else
	{
		return std::unique_ptr< vulkan_staging_manager >(
			new vulkan_staging_manager( cfg ) );
	}

	return nullptr;
}

vulkan_staging_manager::~vulkan_staging_manager()
{}

vulkan_staging_manager::vulkan_staging_manager( const config& cfg )
	: _cfg( cfg )
{}

vulkan_resource* vulkan_staging_manager::resolve( const record_ref& ref )
{
	if ( this != ref.recording_manager )
	{
		LOG_CRITICAL( "record does not belong to this barrier manager" );
	}
	else
	{
		return _pending_resources[ref.record_index];
	}

	return nullptr;
}
