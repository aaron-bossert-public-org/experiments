
#include "vulkan/sync/vulkan_job_dependencies.h"

#include "vulkan/sync/vulkan_dependency.h"

using namespace igpu;


void vulkan_job_dependencies::activate_read_hazard(
	vulkan_dependency* dependency )
{
	auto& state = job_dependency_state();

	size_t write_index = ( size_t )( dependency - state.write_deps.data() );
	size_t read_index = ( size_t )( dependency - state.read_deps.data() );

	if ( write_index < state.write_deps.size() )
	{
		LOG_CRITICAL( "cannot activate write dependency as read dependency" );
	}
	else if ( read_index >= state.read_deps.size() )
	{
		LOG_CRITICAL( "read dependency does not belong to these job dependencies" );
	}
	else if ( !dependency->active() )
	{
		dependency->active( true );
		state.read_hazards.push_back( dependency );
	}
}

void vulkan_job_dependencies::process_dependencies(
	vulkan_barrier_manager* barrier_manager )
{
	auto& state = job_dependency_state();
	for ( auto& write_dependency : state.write_deps )
	{
		barrier_manager->process_dependency( &write_dependency );
	}

	for ( auto* read_hazard : state.read_hazards )
	{
		barrier_manager->process_dependency( read_hazard );
	}

	state.read_hazards.clear();
}

bool vulkan_job_dependencies::validate_barriers() const
{
	auto& state = job_dependency_state();
	bool all_valid = true;

	for ( auto* deps : {
			  &state.write_deps,
			  &state.read_deps,
		  } )
	{
		for ( const vulkan_dependency& dep : *deps )
		{
			if ( false ==
				 dep.resource().validate_barrier(
					 dep.layout(),
					 dep.job_scope() ) )
			{
				all_valid = false;
			}
		}
	}

	return all_valid;
}
