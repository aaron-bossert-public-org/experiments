
#include "vulkan/sync/vulkan_job.h"

#include "vulkan/sync/vulkan_barrier_manager.h"
#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_fence.h"
#include "vulkan/sync/vulkan_job_dependencies.h"

using namespace igpu;

void vulkan_job::start_recording_barriers()
{
	auto& state = job_state();

	ASSERT_CONTEXT( !state.recording );
	state.recording = true;
	state.recorded_dependencies.clear();
}

void vulkan_job::record_barriers( vulkan_job_dependencies* dependencies )
{
	auto& state = job_state();
	ASSERT_CONTEXT( state.recording );
	state.recorded_dependencies.push_back( dependencies );
}

void vulkan_job::submit_recorded_barriers(
	const scoped_ptr< vulkan_queue >& queue,
	vulkan_barrier_manager* barrier_manager )
{
	auto& state = job_state();
	ASSERT_CONTEXT( state.recording );
	barrier_manager->start_recording_barriers();

	for ( auto* dependencies : state.recorded_dependencies )
	{
		dependencies->record_dependencies( barrier_manager );
	}

	barrier_manager->submit_recorded_barriers( queue );

#if ATMOS_DEBUG
	for ( auto* dependencies : state.recorded_dependencies )
	{
		dependencies->validate_barriers();
	}
#endif

	state.recording = false;
}

void vulkan_job::wait_on_fence()
{
	auto& state = job_state();
	if ( const auto& fence = state.fence )
	{
		fence->wait( state.fence_submit_index );
	}

	state.fence = nullptr;
}
void vulkan_job::fence( const scoped_ptr< vulkan_fence >& fence )
{
	auto& state = job_state();
	state.fence = fence;
	state.fence_submit_index = fence->submit_index();
}
