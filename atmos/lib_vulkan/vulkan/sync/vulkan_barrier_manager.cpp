
#include "vulkan/sync/vulkan_barrier_manager.h"

#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_queue.h"
#include "vulkan/sync/vulkan_resource.h"
#include "vulkan/sync/vulkan_semaphore.h"
#include "vulkan/sync/vulkan_synchronization.h"

#include "framework/logging/log.h"

#include <array>

using namespace igpu;

void vulkan_barrier_manager::start_dependency_barriers()
{
	if ( _recording_barriers )
	{
		LOG_CRITICAL(
			"finish_dependency_barriers has not been called since last "
			"start_dependency_barriers" );
	}
	else if ( 0 != _pending_records.size() )
	{
		LOG_CRITICAL(
			"_pending_records has not been cleared since last "
			"start_dependency_barriers" );
	}

	_recording_barriers = true;
}

void vulkan_barrier_manager::record_dependency( vulkan_dependency* dependency )
{
	if ( !dependency )
	{
		LOG_CRITICAL( "dependency is null" );
	}
	else if ( !_recording_barriers )
	{
		LOG_CRITICAL(
			"start_dependency_barriers has not been called since last "
			"finish_dependency_barriers" );
	}
	else
	{
		vulkan_resource& resource = dependency->resource();

		if ( !resource.barrier_manager_record() )
		{
			_pending_records.push_back( {
				&resource,
				dependency->layout(),
				dependency->job_scope(),
			} );

			resource.barrier_manager_record( &_pending_records.back() );
		}
		else if (
			record* record = resolve( resource.barrier_manager_record() ) )
		{
			if ( record->layout != dependency->layout() )
			{
				LOG_CRITICAL(
					"all dependencies on the same image resource in a job must "
					"agree "
					"on the same image layout. current layout(%s)\nrequested "
					"layout(%s)",
					debug::to_string( record->layout ).c_str(),
					debug::to_string( dependency->layout() ).c_str() );
			}
			else
			{
				if ( record->job_scope.is_writable() ||
					 dependency->job_scope().is_writable() )
				{
					LOG_CRITICAL(
						"dependency requests writable access to a resource "
						"that has multiple dependencies within the same job. "
						"make sure that if there are multiple dependencies on "
						"a resource within a job, they are all read only. the "
						"reading and writing of this resource within this job "
						"will not be synchronized." );
				}

				record->job_scope =
					record->job_scope.combined( dependency->job_scope() );
			}
		}
	}
}

void vulkan_barrier_manager::finish_dependency_barriers(
	const scoped_ptr< vulkan_queue >& queue,
	scoped_ptr< vulkan_fence >* out_fence )
{
	if ( !_recording_barriers )
	{
		LOG_CRITICAL(
			"start_dependency_barriers has not been called since last "
			"finish_dependency_barriers" );
	}
	else
	{
		_recording_barriers = false;

		for ( record& record : _pending_records )
		{
			vulkan_resource* resource = record.resource;
			resource->on_barrier( this, queue );
			resource->barrier_manager_record( nullptr );
		}

		_pending_records.clear();

		size_t compact_dst = _cfg.synchronization->compact_queue_family_index(
			queue->cfg().family_index );

		// currently there is not an expectation to have more than 4 queues
		// (graphics/present/transfer/compute)
		size_t q = 0;
		std::array< VkSemaphore, 4 > wait_sem;
		std::array< VkSemaphore, 4 > signal_sem;
		std::array< VkPipelineStageFlags, 4 > transfer_stages;

		for ( size_t i = 1; i < _barriers.size(); ++i )
		{
			auto compact_src = ( compact_dst + i ) % _barriers.size();
			auto barrier = _barriers[compact_src];

			if ( barrier.buffer_memory_barriers.size() ||
				 barrier.image_memory_barriers.size() )
			{
				// delayed initialization of transfer semaphores
				transfer_semaphores& trans_sem =
					transfer_semaphores_for_queues( compact_src, compact_dst );

				if ( trans_sem.signal )
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

				wait_sem[q] = trans_sem.wait->vk_semaphore();
				signal_sem[q] = trans_sem.signal->vk_semaphore();
				transfer_stages[q] = barrier.transfer_stages;

				VkPipelineStageFlags top_of_pipe =
					VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

				_cfg.synchronization->compact_queues()[compact_src]
					->one_time_command(
						wait ? 1 : 0,
						&wait,
						&top_of_pipe,
						[&]( const vulkan_command_buffer& command_buffer ) {
							vkCmdPipelineBarrier(
								command_buffer.vk_cmds(),
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
						1,
						&signal_sem[q] );
				++q;
			}
		}

		auto barrier = _barriers[compact_dst];
		queue->one_time_command(
			(uint32_t)q,
			signal_sem.data(),
			transfer_stages.data(),
			[&]( const vulkan_command_buffer& command_buffer ) {
				vkCmdPipelineBarrier(
					command_buffer.vk_cmds(),
					barrier.src_stages,
					barrier.dst_stages,
					(VkDependencyFlags)0,
					0,
					nullptr,
					(uint32_t)barrier.buffer_memory_barriers.size(),
					barrier.buffer_memory_barriers.data(),
					(uint32_t)barrier.image_memory_barriers.size(),
					barrier.image_memory_barriers.data() );

				if ( out_fence )
				{
					*out_fence = command_buffer.fence();
				}
			},
			(uint32_t)q,
			wait_sem.data() );

		for ( auto& queue_barrier : _barriers )
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
			"start_dependency_barriers has not been called since last "
			"finish_dependency_barriers" );
	}
	else if ( 0 == _pending_records.size() )
	{
		LOG_CRITICAL(
			"push_barrier must be called indirectly via "
			"finish_dependency_barriers" );
	}
	else
	{
		auto& queue_barrier = this->barrier( target_queue_family_index );

		queue_barrier.src_stages |= src_stages;
		queue_barrier.dst_stages |= dst_stages;
		queue_barrier.image_memory_barriers.push_back( barrier );

		if ( barrier.srcQueueFamilyIndex != target_queue_family_index )
		{
			auto& src_queue_barrier =
				this->barrier( target_queue_family_index );
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
			"start_dependency_barriers has not been called since last "
			"finish_dependency_barriers" );
	}
	else if ( 0 == _pending_records.size() )
	{
		LOG_CRITICAL(
			"push_barrier must be called indirectly via "
			"finish_dependency_barriers" );
	}
	else
	{
		auto& queue_barrier = this->barrier( target_queue_family_index );

		queue_barrier.src_stages |= src_stages;
		queue_barrier.dst_stages |= dst_stages;
		queue_barrier.buffer_memory_barriers.push_back( barrier );

		if ( barrier.srcQueueFamilyIndex != target_queue_family_index )
		{
			auto& src_queue_barrier =
				this->barrier( target_queue_family_index );
			src_queue_barrier.transfer_stages |= dst_stages;
		}
	}
}

std::unique_ptr< vulkan_barrier_manager > vulkan_barrier_manager::make(
	const config& cfg )
{
	if ( !cfg.synchronization )
	{
		LOG_CRITICAL( "synchronization is null" );
	}
	else
	{
		const auto& queues = cfg.synchronization->compact_queues();

		// construct vector of vector of null pointers, semaphores will be
		// constructed lazily on demand
		std::vector< std::vector< transfer_semaphores > > trans_semaphores(
			queues.size() );

		for ( auto& vec : trans_semaphores )
		{
			vec.resize( queues.size() );
		}

		auto barrier_manager = std::unique_ptr< vulkan_barrier_manager >(
			new vulkan_barrier_manager( cfg, std::move( trans_semaphores ) ) );

		return barrier_manager;
	}

	return nullptr;
}

vulkan_barrier_manager ::~vulkan_barrier_manager()
{}

vulkan_barrier_manager::vulkan_barrier_manager(
	const config& cfg,
	std::vector< std::vector< transfer_semaphores > >&& trans_semaphores )
	: _cfg( cfg )
	, _transfer_semaphores( std::move( trans_semaphores ) )
	, _barriers( cfg.synchronization->compact_queues().size() )
{}

vulkan_barrier_manager::record* vulkan_barrier_manager::resolve(
	const record* record )
{
	size_t index = record - _pending_records.data();
	if ( index > _pending_records.size() )
	{
		LOG_CRITICAL( "record does not belong to this barrier manager" );
		return nullptr;
	}
	return &_pending_records[index];
}

vulkan_barrier_manager::pipeline_barrier& vulkan_barrier_manager::barrier(
	uint32_t queue_family_index )
{
	size_t compact_index =
		_cfg.synchronization->compact_queue_family_index( queue_family_index );

	return _barriers[compact_index];
}

const vulkan_barrier_manager::pipeline_barrier& vulkan_barrier_manager::barrier(
	uint32_t queue_family_index ) const
{
	size_t compact_index =
		_cfg.synchronization->compact_queue_family_index( queue_family_index );

	return _barriers[compact_index];
}

vulkan_barrier_manager::transfer_semaphores& vulkan_barrier_manager::
	transfer_semaphores_for_queues(
		size_t src_compact_queue_index,
		size_t dst_compact_queue_index )
{
	return _transfer_semaphores[src_compact_queue_index]
							   [dst_compact_queue_index];
}
