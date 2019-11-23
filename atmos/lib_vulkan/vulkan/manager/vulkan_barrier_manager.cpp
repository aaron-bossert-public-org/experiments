
#include "vulkan/manager/vulkan_barrier_manager.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/manager/vulkan_managers.h"
#include "vulkan/manager/vulkan_queue_manager.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_queue.h"
#include "vulkan/sync/vulkan_resource.h"
#include "vulkan/sync/vulkan_semaphore.h"
#include "vulkan/texture/vulkan_image.h"

#include "framework/logging/log.h"

#include <array>

#define ATMOS_DEBUG_BARRIERS ATMOS_DEBUG

using namespace igpu;

namespace
{
	template < typename T >
	void push_one_cb(
		const scoped_ptr< vulkan_queue > queue,
		T&& builder,
		uint32_t wait_count = 0,
		VkSemaphore* p_wait_semaphores = nullptr,
		VkPipelineStageFlags* p_wait_stages = nullptr,
		uint32_t signal_count = 0,
		VkSemaphore* p_signal_semaphores = nullptr )
	{
		vulkan_command_buffer command_buffer( {
			queue->command_pool(),
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		} );

		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkCommandBuffer vk_cmds = command_buffer.vk_cmds();

		vkBeginCommandBuffer( vk_cmds, &begin_info );
		builder( vk_cmds );
		vkEndCommandBuffer( vk_cmds );

		auto* p_commands = &command_buffer;
		queue->push_commands(
			wait_count,
			p_wait_semaphores,
			p_wait_stages,
			1,
			&p_commands,
			signal_count,
			p_signal_semaphores );
	}
}

void vulkan_barrier_manager::submit_frame_job_barriers(
	const scoped_ptr< vulkan_queue >& queue,
	size_t job_barrier_count,
	const frame_job_barrier* job_barriers )
{
	start_recording_barriers();

	for ( const auto* job_barrier = job_barriers;
		  job_barrier < job_barriers + job_barrier_count;
		  ++job_barrier )
	{
		record_barrier(
			job_barrier->resource(),
			job_barrier->layout(),
			job_barrier->job_scope() );
	}

	push_recorded_barriers( queue );

#if ATMOS_DEBUG_BARRIERS
	for ( const auto* job_barrier = job_barriers;
		  job_barrier < job_barriers + job_barrier_count;
		  ++job_barrier )
	{
		job_barrier->resource()->validate_barrier(
			job_barrier->layout(),
			job_barrier->job_scope() );
	}
#endif
}

void vulkan_barrier_manager::start_recording_barriers()
{
	if ( _recording_barriers )
	{
		LOG_CRITICAL(
			"submit_recorded_barriers has not been called since last "
			"start_recording_barriers" );
	}
	else if ( 0 != _pending_records.size() )
	{
		LOG_CRITICAL(
			"_pending_records has not been cleared since last "
			"start_recording_barriers" );
	}

	_recording_barriers = true;
}

void vulkan_barrier_manager::record_barrier(
	vulkan_resource* resource,
	VkImageLayout layout,
	const vulkan_job_scope& job_scope )
{
	if ( !resource )
	{
		LOG_CRITICAL( "resource is null" );
	}
	else if ( !_recording_barriers )
	{
		LOG_CRITICAL(
			"start_recording_barriers has not been called since last "
			"submit_recorded_barriers" );
	}
	else
	{
		const auto& record_ref = resource->barrier_record_ref();
		if ( !record_ref.recording_manager )
		{
			resource->barrier_record_ref( { this, _pending_records.size() } );

			_pending_records.push_back( {
				resource,
				layout,
				job_scope,
			} );
		}
		else if ( record* record = resolve( record_ref ) )
		{
			if ( record->layout != layout )
			{
				LOG_CRITICAL(
					"all dependencies on the same image resource in a job must "
					"agree "
					"on the same image layout. current layout(%s)\nrequested "
					"layout(%s)",
					debug::to_string( record->layout ).c_str(),
					debug::to_string( layout ).c_str() );
			}
			else
			{
				if ( record->job_scope.is_writable() ||
					 job_scope.is_writable() )
				{
					LOG_CRITICAL(
						"dependency requests writable access to a resource "
						"that has multiple dependencies within the same job. "
						"make sure that if there are multiple dependencies on "
						"a resource within a job, they are all read only. the "
						"reading and writing of this resource within this job "
						"will not be synchronized." );
				}

				record->job_scope = record->job_scope.combined( job_scope );
			}
		}
	}
}

void vulkan_barrier_manager::push_recorded_barriers(
	const scoped_ptr< vulkan_queue >& queue )
{
	if ( !_recording_barriers )
	{
		LOG_CRITICAL(
			"start_recording_barriers has not been called since last "
			"submit_recorded_barriers" );
	}
	else
	{
		_recording_barriers = false;

		for ( record& record : _pending_records )
		{
			vulkan_resource* resource = record.resource;
			resource
				->on_barrier( this, queue, record.layout, record.job_scope );
			resource->barrier_record_ref( {} );
		}

		_pending_records.clear();

		size_t dst_family = queue->cfg().family_index;

		// currently there is not an expectation to have more than 4 queues
		// (graphics/present/transfer/compute)
		size_t queue_transfers = 0;
		std::array< VkSemaphore, 4 > wait_sem;
		std::array< VkSemaphore, 4 > signal_sem;
		std::array< VkPipelineStageFlags, 4 > transfer_stages;

		for ( size_t i = 1; i < _barrier_family_table.size(); ++i )
		{
			auto src_family = ( dst_family + i ) % _barrier_family_table.size();
			auto release = _barrier_family_table[src_family];

			if ( release.buffer_memory_barriers.size() ||
				 release.image_memory_barriers.size() )
			{
				// delayed initialization of transfer semaphores
				transfer_semaphores& trans_sem =
					_semaphore_family_tables[src_family][dst_family];

				if ( !trans_sem.signal )
				{
					trans_sem.signal =
						vulkan_semaphore::make( { _cfg.device } );
				}

				// we may not have transfered between this source and dst, so we
				// may not have to wait this first time
				VkSemaphore wait = nullptr;
				if ( !trans_sem.wait )
				{
					trans_sem.wait = vulkan_semaphore::make( { _cfg.device } );
				}
				else
				{
					wait = trans_sem.wait->vk_semaphore();
				}

				wait_sem[queue_transfers] = trans_sem.wait->vk_semaphore();
				signal_sem[queue_transfers] = trans_sem.signal->vk_semaphore();
				transfer_stages[queue_transfers] = release.transfer_stages;

				VkPipelineStageFlags top_of_pipe =
					VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

				const auto& transfer_src_queue =
					_cfg.queue_manager->cfg().queue_family_table[src_family];
				push_one_cb(
					transfer_src_queue,
					[&]( VkCommandBuffer vk_cmds ) {
						vkCmdPipelineBarrier(
							vk_cmds,
							release.src_stages,
							release.dst_stages,
							(VkDependencyFlags)0,
							0,
							nullptr,
							(uint32_t)release.buffer_memory_barriers.size(),
							release.buffer_memory_barriers.data(),
							(uint32_t)release.image_memory_barriers.size(),
							release.image_memory_barriers.data() );
					},
					wait ? 1 : 0,
					&wait,
					&top_of_pipe,
					1,
					&signal_sem[queue_transfers] );
				transfer_src_queue->submit_pending();
				++queue_transfers;
			}
		}

		auto barrier = _barrier_family_table[dst_family];
		if ( barrier.buffer_memory_barriers.size() ||
			 barrier.image_memory_barriers.size() )
		{
			push_one_cb(
				queue,
				[&]( VkCommandBuffer vk_cmds ) {
					vkCmdPipelineBarrier(
						vk_cmds,
						barrier.src_stages,
						barrier.dst_stages,
						(VkDependencyFlags)0,
						0,
						nullptr,
						(uint32_t)barrier.buffer_memory_barriers.size(),
						barrier.buffer_memory_barriers.data(),
						(uint32_t)barrier.image_memory_barriers.size(),
						barrier.image_memory_barriers.data() );
				},
				(uint32_t)queue_transfers,
				signal_sem.data(),
				transfer_stages.data(),
				(uint32_t)queue_transfers,
				wait_sem.data() );
			queue->submit_pending();
		}


		for ( auto& queue_barrier : _barrier_family_table )
		{
			queue_barrier = {};
		}
	}
}

void vulkan_barrier_manager::push_barrier(
	uint32_t target_queue_family_index,
	VkPipelineStageFlags src_stages,
	VkPipelineStageFlags dst_stages,
	const VkImageMemoryBarrier& barrier )
{
	if ( _recording_barriers )
	{
		LOG_CRITICAL(
			"start_recording_barriers has not been called since last "
			"submit_recorded_barriers" );
	}
	else if ( 0 == _pending_records.size() )
	{
		LOG_CRITICAL(
			"push_barrier must be called indirectly via "
			"submit_recorded_barriers" );
	}
	else
	{
		ASSERT_CONTEXT( src_stages || barrier.oldLayout != barrier.newLayout );
		ASSERT_CONTEXT( dst_stages );
		ASSERT_CONTEXT( barrier.srcAccessMask || barrier.dstAccessMask );
		ASSERT_CONTEXT( barrier.oldLayout < VK_IMAGE_LAYOUT_MAX_ENUM );
		ASSERT_CONTEXT( barrier.newLayout < VK_IMAGE_LAYOUT_MAX_ENUM );
		ASSERT_CONTEXT( barrier.image );
		ASSERT_CONTEXT( barrier.subresourceRange.layerCount );
		ASSERT_CONTEXT( barrier.subresourceRange.levelCount );
		ASSERT_CONTEXT(
			target_queue_family_index < _barrier_family_table.size() );
		ASSERT_CONTEXT(
			barrier.srcQueueFamilyIndex < _barrier_family_table.size() );
		ASSERT_CONTEXT(
			barrier.dstQueueFamilyIndex < _barrier_family_table.size() );

		auto& queue_barrier = _barrier_family_table[target_queue_family_index];

		queue_barrier.src_stages |= src_stages;
		queue_barrier.dst_stages |= dst_stages;
		queue_barrier.image_memory_barriers.push_back( barrier );

		if ( barrier.srcQueueFamilyIndex != target_queue_family_index )
		{
			auto& src_queue_barrier =
				_barrier_family_table[barrier.srcQueueFamilyIndex];

			src_queue_barrier.transfer_stages |= dst_stages;
		}
	}
}

void vulkan_barrier_manager::push_barrier(
	uint32_t target_queue_family_index,
	VkPipelineStageFlags src_stages,
	VkPipelineStageFlags dst_stages,
	const VkBufferMemoryBarrier& barrier )
{
	if ( _recording_barriers )
	{
		LOG_CRITICAL(
			"start_recording_barriers has not been called since last "
			"submit_recorded_barriers" );
	}
	else if ( 0 == _pending_records.size() )
	{
		LOG_CRITICAL(
			"push_barrier must be called indirectly via "
			"submit_recorded_barriers" );
	}
	else
	{
		ASSERT_CONTEXT( src_stages );
		ASSERT_CONTEXT( dst_stages );
		ASSERT_CONTEXT( barrier.srcAccessMask || barrier.dstAccessMask );
		ASSERT_CONTEXT( barrier.buffer );
		ASSERT_CONTEXT( barrier.size );
		ASSERT_CONTEXT(
			target_queue_family_index < _barrier_family_table.size() );
		ASSERT_CONTEXT(
			barrier.srcQueueFamilyIndex < _barrier_family_table.size() );
		ASSERT_CONTEXT(
			barrier.dstQueueFamilyIndex < _barrier_family_table.size() );

		auto& queue_barrier = _barrier_family_table[target_queue_family_index];

		queue_barrier.src_stages |= src_stages;
		queue_barrier.dst_stages |= dst_stages;
		queue_barrier.buffer_memory_barriers.push_back( barrier );

		if ( barrier.srcQueueFamilyIndex != target_queue_family_index )
		{
			auto& src_queue_barrier =
				_barrier_family_table[barrier.srcQueueFamilyIndex];
			src_queue_barrier.transfer_stages |= dst_stages;
		}
	}
}

std::unique_ptr< vulkan_barrier_manager > vulkan_barrier_manager::make(
	const config& cfg )
{
	if ( !cfg.queue_manager )
	{
		LOG_CRITICAL( "queue manager is null" );
	}
	else
	{
		// construct vector of vector of null pointers, semaphores will be
		// constructed lazily on demand
		std::vector< std::vector< transfer_semaphores > >
			semaphore_family_tables(
				cfg.queue_manager->cfg().queue_family_table.size() );

		for ( auto& vec : semaphore_family_tables )
		{
			vec.resize( semaphore_family_tables.size() );
		}

		auto barrier_manager = std::unique_ptr< vulkan_barrier_manager >(
			new vulkan_barrier_manager(
				cfg,
				std::move( semaphore_family_tables ) ) );

		return barrier_manager;
	}

	return nullptr;
}

vulkan_barrier_manager ::~vulkan_barrier_manager()
{}

vulkan_barrier_manager::vulkan_barrier_manager(
	const config& cfg,
	std::vector< std::vector< transfer_semaphores > >&&
		semaphore_family_tables )
	: _cfg( cfg )
	, _semaphore_family_tables( std::move( semaphore_family_tables ) )
	, _barrier_family_table(
		  cfg.queue_manager->cfg().queue_family_table.size() )
{}

vulkan_barrier_manager::record* vulkan_barrier_manager::resolve(
	const record_ref& ref )
{
	if ( this != ref.recording_manager )
	{
		LOG_CRITICAL( "record does not belong to this barrier manager" );
	}
	else
	{
		return &_pending_records[ref.record_index];
	}

	return nullptr;
}