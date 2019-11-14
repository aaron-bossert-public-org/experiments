
#include "vulkan/sync/vulkan_barrier_manager.h"

#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_resource.h"
#include "vulkan/sync/vulkan_synchronization.h"

#include "framework/logging/log.h"
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
				if ( !record->job_scope.is_read_only() ||
					 !dependency->job_scope().is_read_only() )
				{
					LOG_CRITICAL(
						"dependency requests writable access to a resource "
						"that "
						"has multiple dependencies within the same job. make "
						"sure "
						"that if there are multiple dependencies on a resource "
						"within a job, they are all read only. the reading and "
						"writing of this resource within this job will not be "
						"synchronized." );
				}

				record->job_scope =
					record->job_scope.combined( dependency->job_scope() );
			}
		}
	}
}

void vulkan_barrier_manager::finish_dependency_barriers(
	const scoped_ptr< vulkan_queue >& queue )
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

		for ( auto& barrier : _barriers )
		{
			barrier = {};
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
		auto& pipeline_barrier = this->barrier( target_queue_family_index );

		pipeline_barrier.src_stages |= src_stages;
		pipeline_barrier.dst_stages |= dst_stages;
		pipeline_barrier.image_memory_barriers.push_back( barrier );
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
		auto& pipeline_barrier = this->barrier( target_queue_family_index );

		pipeline_barrier.src_stages |= src_stages;
		pipeline_barrier.dst_stages |= dst_stages;
		pipeline_barrier.buffer_memory_barriers.push_back( barrier );
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
		return std::unique_ptr< vulkan_barrier_manager >(
			new vulkan_barrier_manager( cfg ) );
	}

	return nullptr;
}

vulkan_barrier_manager::vulkan_barrier_manager( const config& cfg )
	: _cfg( cfg )
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