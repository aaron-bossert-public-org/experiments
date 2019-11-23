
#include "vulkan/sync/vulkan_job.h"

#include "vulkan/manager/vulkan_barrier_manager.h"
#include "vulkan/manager/vulkan_managers.h"
#include "vulkan/manager/vulkan_staging_manager.h"
#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_job_dependencies.h"
#include "vulkan/sync/vulkan_poset_fence.h"

using namespace igpu;

#define ATMOS_DEBUG_BARRIERS ATMOS_DEBUG

void vulkan_job::activate_dependencies( vulkan_job_dependencies* dependencies )
{
	job_state().activated_dependencies.push_back( dependencies );
}

void vulkan_job::deactivate_dependencies(
	vulkan_job_dependencies* dependencies )
{
	auto& state = job_state();
	auto& activated_deps = state.activated_dependencies;

	for ( auto& activated : activated_deps )
	{
		if ( activated == dependencies )
		{
			activated = activated_deps.back();
			activated_deps.pop_back();
			return;
		}
	}
}

void vulkan_job::submit_activated_dependencies(
	const scoped_ptr< vulkan_queue >& queue,
	vulkan_managers* managers )
{
	submit_staged_copies( managers->cfg().staging.get() );

	submit_dependency_barriers( queue, managers->cfg().barrier.get() );

	auto& state = job_state();
	auto& activated_deps = state.activated_dependencies;

	for ( size_t i = activated_deps.size(); i-- > 0; )
	{
		if ( !activated_deps[i]->is_activated() )
		{
			activated_deps[i] = activated_deps.back();
			activated_deps.pop_back();
		}
	}
}

void vulkan_job::submit_staged_copies( vulkan_staging_manager* staging_manager )
{
	auto& state = job_state();
	auto& activated_deps = state.activated_dependencies;

	staging_manager->start_recording_transfers();

	for ( auto* dependencies : activated_deps )
	{
		dependencies->record_transfers( staging_manager );
	}

	staging_manager->push_pending_transfers();
}

void vulkan_job::submit_dependency_barriers(
	const scoped_ptr< vulkan_queue >& queue,
	vulkan_barrier_manager* barrier_manager )
{
	auto& state = job_state();
	auto& activated_deps = state.activated_dependencies;

#if ATMOS_DEBUG_BARRIERS
	for ( auto* dependencies : activated_deps )
	{
		dependencies->validate_hazards();
	}
#endif

	barrier_manager->start_recording_barriers();

	for ( auto* dependencies : activated_deps )
	{
		dependencies->record_barriers( barrier_manager );
	}

	barrier_manager->push_recorded_barriers( queue );

#if ATMOS_DEBUG_BARRIERS
	for ( auto* dependencies : activated_deps )
	{
		dependencies->validate_barriers();
	}
#endif
}

void vulkan_job::wait_on_fence()
{
	auto& state = job_state();
	if ( const auto& fence = state.fence )
	{
		fence->wait_or_skip( state.fence_submit_index );
	}

	state.fence = nullptr;
}
void vulkan_job::fence( const scoped_ptr< vulkan_poset_fence >& fence )
{
	auto& state = job_state();
	state.fence = fence;
	state.fence_submit_index = fence->submit_index();
}
