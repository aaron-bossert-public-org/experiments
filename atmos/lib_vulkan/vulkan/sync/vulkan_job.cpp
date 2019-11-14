
#include "vulkan/sync/vulkan_job.h"

#include "vulkan/sync/vulkan_barrier_manager.h"
#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_fence.h"
#include "vulkan/sync/vulkan_job_dependencies.h"

using namespace igpu;

void vulkan_job::begin_record_cmds()
{
	job_state().recorded_dependencies.clear();
}

void vulkan_job::on_record_cmds( vulkan_job_dependencies* dependencies )
{
	job_state().recorded_dependencies.push_back( dependencies );
}
void vulkan_job::barrier_recorded_commands(
	const scoped_ptr< vulkan_queue >& queue,
	vulkan_barrier_manager* barrier_manager )
{
	auto& state = job_state();
	barrier_manager->start_dependency_barriers();

	for ( auto* dependencies : state.recorded_dependencies )
	{
		dependencies->record_dependencies( barrier_manager );
	}

	barrier_manager->finish_dependency_barriers( queue );

#if ATMOS_DEBUG
	for ( auto* dependencies : state.recorded_dependencies )
	{
		dependencies->validate_barriers();
	}
#endif
}

void vulkan_job::wait_on_fence() const
{
	if ( const auto& fence = job_state().fence )
	{
		fence->wait();
	}
}
void vulkan_job::fence( const std::shared_ptr< vulkan_fence >& fence )
{
	job_state().fence = fence;
}
