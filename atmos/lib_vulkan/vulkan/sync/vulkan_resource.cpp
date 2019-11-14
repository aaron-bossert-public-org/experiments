
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
			dependency->job_dependencies().activate_read_hazard( dependency );
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

void vulkan_resource::wait_pending_jobs() const
{
	for ( auto* deps : {
			  &resource_state().read_deps,
			  &resource_state().write_deps,
		  } )
	{
		for ( vulkan_dependency* dependency : *deps )
		{
			dependency->job_dependencies().wait_pending_job();
		}
	}
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
			dependency->job_dependencies().gpu_object_reallocated( dependency );
		}
	}
}

void vulkan_resource::on_barrier(
	vulkan_barrier_manager* barrier_manager,
	const scoped_ptr< vulkan_queue >& queue )
{
	auto& state = this->resource_state();
	auto* record = state.barrier_manager_record;

	uint32_t src_queue_family = state.queue->cfg().family_index;
	uint32_t dst_queue_family = queue->cfg().family_index;

	// if writes are being performed, we tell all subsequent read dependencies
	// to apply barriers before accessing the resource.
	bool is_write = !record->job_scope.is_read_only();
	bool is_transfer = src_queue_family != dst_queue_family;
	bool is_transition = state.layout != record->layout;

	if ( is_write || is_transfer || is_transition )
	{
		if ( is_transfer )
		{
			push_barrier(
				src_queue_family,
				barrier_manager,
				src_queue_family,
				src_queue_family,
				state.layout,
				record->layout,
				state.last_write_scope.combined( state.combined_read_scope ),
				record->job_scope );
		}
		else
		{
			// transfer queue ownership
			vulkan_job_scope transfer_job = {
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				0,
			};

			// hold layout transition until destination queue takes ownership.
			// (should be forcing layout transitions onto the graphics queue)
			push_barrier(
				src_queue_family,
				barrier_manager,
				src_queue_family,
				dst_queue_family,
				state.layout,
				state.layout,
				state.last_write_scope.combined( state.combined_read_scope ),
				transfer_job );

			transfer_job.stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

			push_barrier(
				dst_queue_family,
				barrier_manager,
				src_queue_family,
				dst_queue_family,
				state.layout,
				record->layout,
				transfer_job,
				record->job_scope );

			state.queue = queue;
		}

		// 1st execution scope of subsequent barriers is the current job
		// dependency
		state.last_write_scope = record->job_scope;
		state.layout = record->layout;

		// any subsequent read will be forced to add a barrier
		if ( is_write )
		{
			state.combined_read_scope = {};
		}
		else
		{
			// any subsequent read will be forced to add a barrier if they do
			// not share the same scope as the current barrier (transitions and
			// transfers happen between execution scopes rather than as a part
			// of the second execution scope so read barriers can be avoided
			// after if they share the same job scope as the layout transition)
			state.combined_read_scope = record->job_scope;
		}

		// subsequent jobs depending on read acces to this resource should add
		// barriers if their job scope is not equal to the current job scope
		for ( auto read_dep : state.read_deps )
		{
			if ( !state.combined_read_scope.contains( read_dep->job_scope() ) )
			{
				read_dep->job_dependencies().activate_read_hazard( read_dep );
			}
		}
	}
	else if ( !state.combined_read_scope.contains( record->job_scope ) )
	{
		push_barrier(
			src_queue_family,
			barrier_manager,
			src_queue_family,
			src_queue_family,
			state.layout,
			record->layout,
			state.last_write_scope,
			record->job_scope );

		// 1st execution scope of subsequent barriers is the current job is
		// whatever caused the previous write/transition/transfer
		state.combined_read_scope =
			state.combined_read_scope.combined( record->job_scope );
	}
}

bool vulkan_resource::validate_barrier(
	VkImageLayout layout,
	const vulkan_job_scope& job_scope ) const
{
	bool is_valid = true;
	auto& state = this->resource_state();
	if ( layout != state.layout )
	{
		LOG_CRITICAL(
			"image layout expected to be(%s) but is (%s) instead",
			debug::to_string( layout ).c_str(),
			debug::to_string( state.layout ).c_str() );
		is_valid = false;
	}

	const vulkan_job_scope* current_scope = nullptr;

	if ( job_scope.is_read_only() )
	{
		current_scope = &state.combined_read_scope;
	}
	else
	{
		current_scope = &state.last_write_scope;
		if ( 0 != state.combined_read_scope.stages ||
			 0 != state.combined_read_scope.access )
		{
			LOG_CRITICAL(
				"resource expected to have been made writable but has read "
				"flags active, stages(%s) access(%s)",
				debug::to_stages_string( state.combined_read_scope.stages )
					.c_str(),
				debug::to_access_string( state.combined_read_scope.access )
					.c_str() );
			is_valid = false;
		}
	}

	VkPipelineStageFlags missing_stages =
		job_scope.stages & ~current_scope->stages;
	VkAccessFlags missing_access = job_scope.access & ~current_scope->access;

	if ( 0 != missing_stages )
	{
		LOG_CRITICAL(
			"resource read scope missing stage flags(%s)",
			debug::to_stages_string( missing_stages ).c_str() );
		is_valid = false;
	}
	if ( 0 != missing_access )
	{
		LOG_CRITICAL(
			"resource read scope missing access flags(%s)",
			debug::to_access_string( missing_access ).c_str() );
		is_valid = false;
	}

	return true;
}