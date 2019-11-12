
#include "vulkan/sync/vulkan_barrier_manager.h"

#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_resource.h"

#include "framework/logging/log.h"
using namespace igpu;

void vulkan_barrier_manager::begin_dependencies()
{
	ASSERT_CONTEXT(
		0 == _pending_records.size(),
		"end_dependencies has not been called since the last invocation of "
		"begin_depenencies" );
	_barrier = {};
}

void vulkan_barrier_manager::process_dependency( vulkan_dependency* dependency )
{
	if ( !dependency )
	{
		LOG_CRITICAL( "dependency is null" );
		return;
	}

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
	else if ( record* record = resolve( resource.barrier_manager_record() ) )
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
					"dependency requests writable access to a resource that "
					"has multiple dependencies within the same job. make sure "
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

void vulkan_barrier_manager::end_dependencies(
	const scoped_ptr< vulkan_queue >& queue )
{
	for ( record& record : _pending_records )
	{
		vulkan_resource* resource = record.resource;
		resource->on_barrier( this, queue );
		resource->barrier_manager_record( nullptr );
	}

	_pending_records.clear();
}

void vulkan_barrier_manager::push_barrier(
	VkPipelineStageFlags src_stages,
	VkPipelineStageFlags dst_stages,
	const VkImageMemoryBarrier& barrier )
{
	_barrier.src_stages |= src_stages;
	_barrier.dst_stages |= dst_stages;
	_barrier.image_memory_barriers.push_back( barrier );
}
void vulkan_barrier_manager::push_barrier(
	VkPipelineStageFlags src_stages,
	VkPipelineStageFlags dst_stages,
	const VkBufferMemoryBarrier& barrier )
{
	_barrier.src_stages |= src_stages;
	_barrier.dst_stages |= dst_stages;
	_barrier.buffer_memory_barriers.push_back( barrier );
}

const vulkan_barrier_manager::pipeline_barrier& vulkan_barrier_manager::
	barrier() const
{
	LOG_CRITICAL(
		"queue ownership transfers not implemented, they can probably be "
		"handled by the object invoking this function" );
	ASSERT_CONTEXT(
		0 == _pending_records.size(),
		"barrier will not be generated until end_dependencies is called" );
	return _barrier;
}


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