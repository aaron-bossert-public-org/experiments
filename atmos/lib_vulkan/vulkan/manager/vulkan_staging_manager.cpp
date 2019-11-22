
#include "vulkan/manager/vulkan_staging_manager.h"

#include "vulkan/manager/vulkan_barrier_manager.h"
#include "vulkan/manager/vulkan_queue_manager.h"
#include "vulkan/sync/vulkan_resource.h"

#include "framework/logging/log.h"

using namespace igpu;
//
// namespace
//{
//	template < typename T >
//	void one_time_command(
//		const scoped_ptr< vulkan_queue > queue,
//		T&& builder,
//		uint32_t wait_count = 0,
//		const VkSemaphore* p_wait_semaphores = nullptr,
//		const VkPipelineStageFlags* p_wait_stages = nullptr,
//		uint32_t signal_count = 0,
//		const VkSemaphore* p_signal_semaphores = nullptr,
//		std::shared_ptr< vulkan_poset_fence > fence = nullptr )
//	{
//		vulkan_command_buffer command_buffer( {
//			queue->command_pool(),
//			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
//		} );
//
//		VkCommandBufferBeginInfo begin_info = {};
//		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//		VkCommandBuffer vk_cmds = command_buffer.vk_cmds();
//
//		vkBeginCommandBuffer( vk_cmds, &begin_info );
//		builder( vk_cmds );
//		vkEndCommandBuffer( vk_cmds );
//
//		queue->submit_commands(
//			wait_count,
//			p_wait_semaphores,
//			p_wait_stages,
//			1,
//			&command_buffer,
//			signal_count,
//			p_signal_semaphores,
//			fence );
//	}
//}
//
// void vulkan_barrier_manager::submit_frame_job(
//	const scoped_ptr< vulkan_queue > queue,
//	const std::initializer_list< frame_job_barrier >& frame_job_barriers,
//	const std::function< void( VkCommandBuffer ) >& builder )
//{
//	start_recording_barriers();
//
//	for ( const auto& barrier : frame_job_barriers )
//	{
//		record_barrier( barrier.resource, barrier.layout, barrier.job_scope );
//	}
//
//	submit_recorded_barriers( queue );
//
//	one_time_command( queue, builder );
//}


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

void vulkan_staging_manager::push_transfers()
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

		// frame_job_barrier for ( record& record : _pending_resources )
		//{
		//	vulkan_resource* resource = record.resource;
		//	resource->transfer_staged( command_buffer );
		//	resource->transfer_record_ref( {} );
		//}

		//_pending_resources.clear();

		// submit size_t dst_family = queue->cfg().family_index;

		//// currently there is not an expectation to have more than 4 queues
		//// (graphics/present/transfer/compute)
		// size_t queue_transfers = 0;
		// std::array< VkSemaphore, 4 > wait_sem;
		// std::array< VkSemaphore, 4 > signal_sem;
		// std::array< VkPipelineStageFlags, 4 > transfer_stages;

		// for ( size_t i = 1; i < _barrier_family_table.size(); ++i )
		//{
		//	auto src_family = ( dst_family + i ) % _barrier_family_table.size();
		//	auto release = _barrier_family_table[src_family];

		//	if ( release.buffer_memory_barriers.size() ||
		//		 release.image_memory_barriers.size() )
		//	{
		//		// delayed initialization of transfer semaphores
		//		transfer_semaphores& trans_sem =
		//			_semaphore_family_tables[src_family][dst_family];

		//		if ( !trans_sem.signal )
		//		{
		//			trans_sem.signal =
		//				vulkan_semaphore::make( { _cfg.device } );
		//		}

		//		// we may not have transfered between this source and dst, so we
		//		// may not have to wait this first time
		//		VkSemaphore wait = nullptr;
		//		if ( !trans_sem.wait )
		//		{
		//			trans_sem.wait = vulkan_semaphore::make( { _cfg.device } );
		//		}
		//		else
		//		{
		//			wait = trans_sem.wait->vk_semaphore();
		//		}

		//		wait_sem[queue_transfers] = trans_sem.wait->vk_semaphore();
		//		signal_sem[queue_transfers] = trans_sem.signal->vk_semaphore();
		//		transfer_stages[queue_transfers] = release.transfer_stages;

		//		VkPipelineStageFlags top_of_pipe =
		//			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		//		one_time_command(
		//			_cfg.queue_manager->cfg().queue_family_table[src_family],
		//			[&]( VkCommandBuffer vk_cmds ) {
		//				vkCmdPipelineBarrier(
		//					vk_cmds,
		//					release.src_stages,
		//					release.dst_stages,
		//					(VkDependencyFlags)0,
		//					0,
		//					nullptr,
		//					(uint32_t)release.buffer_memory_barriers.size(),
		//					release.buffer_memory_barriers.data(),
		//					(uint32_t)release.image_memory_barriers.size(),
		//					release.image_memory_barriers.data() );
		//			},
		//			wait ? 1 : 0,
		//			&wait,
		//			&top_of_pipe,
		//			1,
		//			&signal_sem[queue_transfers] );

		//		++queue_transfers;
		//	}
		//}

		// auto barrier = _barrier_family_table[dst_family];
		// if ( barrier.buffer_memory_barriers.size() ||
		//	 barrier.image_memory_barriers.size() )
		//{
		//	one_time_command(
		//		queue,
		//		[&]( VkCommandBuffer vk_cmds ) {
		//			vkCmdPipelineBarrier(
		//				vk_cmds,
		//				barrier.src_stages,
		//				barrier.dst_stages,
		//				(VkDependencyFlags)0,
		//				0,
		//				nullptr,
		//				(uint32_t)barrier.buffer_memory_barriers.size(),
		//				barrier.buffer_memory_barriers.data(),
		//				(uint32_t)barrier.image_memory_barriers.size(),
		//				barrier.image_memory_barriers.data() );
		//		},
		//		(uint32_t)queue_transfers,
		//		signal_sem.data(),
		//		transfer_stages.data(),
		//		(uint32_t)queue_transfers,
		//		wait_sem.data() );
		//}


		// for ( auto& queue_barrier : _barrier_family_table )
		//{
		//	queue_barrier = {};
		//}
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
