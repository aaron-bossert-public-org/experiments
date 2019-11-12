
#include "vulkan/sync/vulkan_resource.h"

#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_job_dependencies.h"

#include "framework/utility/scoped_ptr.h"

using namespace igpu;

vulkan_resource::link vulkan_resource::add_read_only_dependency(
	vulkan_dependency* dependency )
{
	if ( !dependency )
	{
		LOG_CRITICAL( "dependency is null" );
	}
	else
	{
		auto& dependencies = resource_state().read_deps;
		auto& dep_scope = resource_state().combined_read_scope;
		const auto& job_scope = dependency->job_scope();

		ASSERT_CONTEXT(
			job_scope.is_read_only(),
			"attemping to add writeable dependency as read only" );

		if ( !dep_scope.contains( job_scope ) )
		{
			dependency->job().activate_read_hazard( dependency );
		}

		dependencies.push_front( dependency );
		return dependencies.begin();
	}

	return {};
}

vulkan_resource::link vulkan_resource::add_writeable_dependency(
	vulkan_dependency* dependency )
{
	if ( !dependency )
	{
		LOG_CRITICAL( "dependency is null" );
	}
	else
	{
		auto& dependencies = resource_state().write_deps;
		const auto& job_scope = dependency->job_scope();

		ASSERT_CONTEXT(
			false == job_scope.is_read_only(),
			"attemping to add read only dependency as writable" );

		dependencies.push_front( dependency );
		return dependencies.begin();
	}

	return {};
}

void vulkan_resource::remove_read_only_dependency(
	const vulkan_resource::link& link )
{
	auto& state = resource_state();
	auto dependency = *link;
	if ( !dependency )
	{
		LOG_CRITICAL( "link points to null dependency" );
	}
	else
	{
		state.read_deps.erase( link );
	}
}

void vulkan_resource::remove_writeable_dependency(
	const vulkan_resource::link& link )
{
	auto& state = resource_state();
	auto dependency = *link;
	if ( !dependency )
	{
		LOG_CRITICAL( "link points to null dependency" );
	}
	else
	{
		state.write_deps.erase( link );
	}
}

const vulkan_barrier_manager::record* vulkan_resource::barrier_manager_record()
{
	return resource_state().barrier_manager_record;
}

void vulkan_resource::barrier_manager_record(
	const vulkan_barrier_manager::record* record )
{
	resource_state().barrier_manager_record = record;
}

void vulkan_resource::on_reallocate_gpu_object()
{
	for ( auto* deps : {
			  &resource_state().read_deps,
			  &resource_state().write_deps,
		  } )
	{
		for ( vulkan_dependency* dependency : *deps )
		{
			dependency->job().on_reallocate_gpu_object( dependency );
		}
	}
}

void vulkan_resource::on_barrier(
	vulkan_barrier_manager* barrier_manager,
	const scoped_ptr< vulkan_queue >& queue )
{
	auto& state = this->resource_state();
	auto* record = state.barrier_manager_record;

	// if writes are being performed, we tell all subsequent read dependencies
	// to apply barriers before accessing the resource.

	if ( !record->job_scope.is_read_only() )
	{
		// 1st execution scope is all previous reads and writes
		// 2nd execution scope is defined by record->job_scope
		push_barrier(
			barrier_manager,
			state.queue,
			queue,
			state.layout,
			record->layout,
			state.last_write_scope.combined( state.combined_read_scope ),
			record->job_scope );

		// 1st execution scope of subsequent reads is the current write
		// job_scope
		state.last_write_scope = record->job_scope;
		state.layout = record->layout;

		// any subsequent read will be forced to add a barrier
		state.combined_read_scope = {};

		// subsequent jobs depending on read acces to this resource should add
		// barriers
		for ( auto read_dep : state.read_deps )
		{
			read_dep->job().activate_read_hazard( read_dep );
		}
	}
	else if ( state.layout != record->layout )
	{
		// 1st execution scope is all previous reads and writes
		// 2nd execution scope is defined by record->job_scope
		push_barrier(
			barrier_manager,
			state.queue,
			queue,
			state.layout,
			record->layout,
			state.last_write_scope.combined( state.combined_read_scope ),
			record->job_scope );

		// 1st execution scope of subsequent reads is the current write
		state.last_write_scope = record->job_scope;
		state.layout = record->layout;

		// any subsequent read will be forced to add a barrier if they do not
		// share the same scope as the current barrier
		// (the transition happens between execution scopes rather than as a
		// part of the second execution scope so read barriers can be avoided
		// after a layout transition if they share the same job scope as the
		// layout transition)
		state.combined_read_scope = record->job_scope;

		// subsequent jobs depending on read acces to this resource should add
		// barriers if their job scope is not equal to the current job scope
		for ( auto read_dep : state.read_deps )
		{
			if ( !state.combined_read_scope.contains( read_dep->job_scope() ) )
			{
				read_dep->job().activate_read_hazard( read_dep );
			}
		}
	}
	else if ( !state.combined_read_scope.contains( record->job_scope ) )
	{
		push_barrier(
			barrier_manager,
			state.queue,
			queue,
			state.layout,
			record->layout,
			state.last_write_scope,
			record->job_scope );

		state.combined_read_scope =
			state.combined_read_scope.combined( record->job_scope );
	}

	state.queue = queue;
}
