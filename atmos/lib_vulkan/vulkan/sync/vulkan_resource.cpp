
#include "vulkan/sync/vulkan_resource.h"

#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_job_dependencies.h"
#include "vulkan/sync/vulkan_queue.h"

#include "framework/utility/scoped_ptr.h"

using namespace igpu;

vulkan_resource::link vulkan_resource::add_dependency(
	vulkan_dependency* dependency )
{
	if ( !dependency )
	{
		LOG_CRITICAL( "dependency is null" );
	}
	else
	{
		if ( has_staged_transfer() )
		{
			dependency->job_dependencies().activate_staged_transfer(
				dependency );
		}

		if ( dependency->job_scope().is_writable() )
		{
			auto& dependencies = resource_state().write_deps;

			dependencies.push_front( dependency );
			return dependencies.begin();
		}
		else
		{
			auto& dependencies = resource_state().read_deps;
			auto& read_scope = resource_state().combined_read_scope;

			if ( !read_scope.contains( dependency->job_scope() ) )
			{
				dependency->job_dependencies().activate_read_hazard(
					dependency );
			}

			dependencies.push_front( dependency );
			return dependencies.begin();
		}
	}

	return {};
}

void vulkan_resource::remove_dependency( const vulkan_resource::link& link )
{
	auto& state = resource_state();
	auto dependency = *link;
	if ( !dependency )
	{
		LOG_CRITICAL( "link points to null dependency" );
	}
	else if ( dependency->job_scope().is_writable() )
	{
		state.write_deps.erase( link );
	}
	else
	{
		state.read_deps.erase( link );
	}
}

const vulkan_barrier_manager::record_ref& vulkan_resource::barrier_record_ref()
	const
{
	return resource_state().barrier_record_ref;
}

void vulkan_resource::barrier_record_ref(
	const vulkan_barrier_manager::record_ref& record_ref )
{
	resource_state().barrier_record_ref = record_ref;
}

const vulkan_staging_manager::record_ref& vulkan_resource::staging_record_ref()
	const
{
	return resource_state().staging_record_ref;
}

void vulkan_resource::staging_record_ref(
	const vulkan_staging_manager::record_ref& record_ref )
{
	resource_state().staging_record_ref = record_ref;
}

ptrdiff_t vulkan_resource::write_count() const
{
	return resource_state().write_count;
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


void vulkan_resource::stage_transfer()
{
	for ( auto* deps : {
			  &resource_state().read_deps,
			  &resource_state().write_deps,
		  } )
	{
		for ( vulkan_dependency* dependency : *deps )
		{
			dependency->job_dependencies().activate_staged_transfer(
				dependency );
		}
	}
}

const scoped_ptr< vulkan_queue >& vulkan_resource::pending_queue() const
{
	return resource_state().queue;
}

void vulkan_resource::reinitialized(
	const scoped_ptr< vulkan_queue >& queue,
	VkImageLayout layout,
	const vulkan_job_scope& job_scope )
{
	ASSERT_CONTEXT( job_scope.stages );
	ASSERT_CONTEXT( is_valid_layout( layout ) );

	auto& state = resource_state();
	state.queue = queue;
	state.layout = layout;
	state.last_write_scope = job_scope;
	state.combined_read_scope = {};

	for ( auto* deps : {
			  &resource_state().read_deps,
			  &resource_state().write_deps,
		  } )
	{
		for ( vulkan_dependency* dependency : *deps )
		{
			dependency->job_dependencies().resource_reinitialized( dependency );
		}
	}
}

void vulkan_resource::on_barrier(
	vulkan_barrier_manager* barrier_manager,
	const scoped_ptr< vulkan_queue >& queue,
	VkImageLayout layout,
	const vulkan_job_scope& job_scope )
{
	auto& state = this->resource_state();

	ASSERT_CONTEXT( (bool)queue );
	ASSERT_CONTEXT( job_scope.validate() );
	ASSERT_CONTEXT( is_valid_layout( layout ) );


	uint32_t dst_queue_family = queue->cfg().family_index;
	uint32_t src_queue_family = dst_queue_family;

	if ( state.queue )
	{
		src_queue_family = state.queue->cfg().family_index;
	}

	// if writes are being performed, we tell all subsequent read
	// dependencies to apply barriers before accessing the resource.
	bool is_write = job_scope.is_writable();
	bool is_transition = state.layout != layout;
	bool is_transfer = src_queue_family != dst_queue_family;
	bool is_skippable = !state.queue && !is_transition;

	state.queue = queue;

	// no barrier necessary if there is no previous job scope
	if ( is_skippable )
	{
		ASSERT_CONTEXT( state.last_write_scope.is_empty() );
		ASSERT_CONTEXT( state.combined_read_scope.is_empty() );
		ASSERT_CONTEXT(
			state.layout == VK_IMAGE_LAYOUT_MAX_ENUM ||
			state.layout == VK_IMAGE_LAYOUT_UNDEFINED );

		state.queue = queue;
		state.layout = layout;
		state.last_write_scope = job_scope;
		state.combined_read_scope =
			job_scope.is_writable() ? vulkan_job_scope{} : job_scope;
	}
	else if ( is_write || is_transfer || is_transition )
	{
		auto prev_complete_scope =
			state.last_write_scope.combined( state.combined_read_scope );
		if ( is_transfer )
		{
			// transfer queue ownership
			vulkan_job_scope transfer_job = {
				job_scope.decorators,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				(VkAccessFlagBits)0,
			};

			// hold layout transition until destination queue takes
			// ownership. (should be forcing layout transitions onto the
			// graphics queue)
			push_barrier(
				src_queue_family,
				barrier_manager,
				src_queue_family,
				dst_queue_family,
				state.layout,
				state.layout,
				prev_complete_scope,
				transfer_job );

			transfer_job.decorators = prev_complete_scope.decorators;
			transfer_job.stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

			push_barrier(
				dst_queue_family,
				barrier_manager,
				src_queue_family,
				dst_queue_family,
				state.layout,
				layout,
				transfer_job,
				job_scope );
		}
		else
		{
			push_barrier(
				dst_queue_family,
				barrier_manager,
				dst_queue_family,
				dst_queue_family,
				state.layout,
				layout,
				prev_complete_scope,
				job_scope );
		}


		// 1st execution scope of subsequent barriers is the current job
		// dependency
		state.queue = queue;
		state.layout = layout;
		state.last_write_scope = job_scope;

		// any subsequent read will be forced to add a barrier
		if ( is_write )
		{
			++state.write_count;
			state.combined_read_scope = {};
		}
		else
		{
			// any subsequent read will be forced to add a barrier if they
			// do not share the same scope as the current barrier
			// (transitions and transfers happen between execution scopes
			// rather than as a part of the second execution scope so read
			// barriers can be avoided after if they share the same job
			// scope as the layout transition)
			state.combined_read_scope = job_scope;
		}

		// subsequent jobs depending on read acces to this resource should
		// add barriers if their job scope is not equal to the current job
		// scope
		for ( auto read_dep : state.read_deps )
		{
			if ( !state.combined_read_scope.contains( read_dep->job_scope() ) )
			{
				read_dep->job_dependencies().activate_read_hazard( read_dep );
			}
		}
	}
	else if ( !state.combined_read_scope.contains( job_scope ) )
	{
		push_barrier(
			dst_queue_family,
			barrier_manager,
			dst_queue_family,
			dst_queue_family,
			state.layout,
			layout,
			state.last_write_scope,
			job_scope );

		// 1st execution scope of subsequent barriers is the current job is
		// whatever caused the previous write/transition/transfer
		state.combined_read_scope =
			state.combined_read_scope.combined( job_scope );
	}
}

bool vulkan_resource::validate_hazard(
	const vulkan_dependency* read_dependency,
	const std::vector< vulkan_dependency* >& read_hazards ) const
{
	bool is_valid = true;
	auto& state = this->resource_state();
	bool needs_layout = state.layout != read_dependency->layout();
	bool needs_barrier =
		!state.combined_read_scope.contains( read_dependency->job_scope() );

	bool is_hazard = read_dependency->is_hazard();
	bool in_hazards = std::find(
						  read_hazards.begin(),
						  read_hazards.end(),
						  read_dependency ) != read_hazards.end();

	if ( is_hazard != in_hazards )
	{
		LOG_CRITICAL(
			is_hazard
				? "resource is marked as a hazard but is not in hazards list"
				: "resource is in hazards list but is not marked as a hazard" );
	}

	if ( needs_layout || needs_barrier )
	{
		if ( !is_hazard || !in_hazards )
		{
			const char* hazard_request = ( needs_layout && needs_barrier )
				? "needs barrier and layout transition"
				: ( needs_layout ? "needs layout transition"
								 : "needs barrier" );


			const char* hazard_state = ( !is_hazard && !in_hazards )
				? "is not marked as a hazard and is not in hazards list"
				: !is_hazard ? "is not marked as a hazard"
							 : "is not added to hazards list";

			LOG_CRITICAL( "resource %s but %s", hazard_request, hazard_state );

			is_valid = false;
		}
	}

	return is_valid;
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

	if ( !job_scope.is_writable() )
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
				"resource expected to have been made writable which "
				"implies "
				"read scope should have been cleared but has read flags "
				"active"
				"flags active, decorators(%s) stages(%s) access(%s)",
				to_string( state.combined_read_scope.decorators ).data(),
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