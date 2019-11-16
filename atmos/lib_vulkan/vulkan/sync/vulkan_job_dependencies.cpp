
#include "vulkan/sync/vulkan_job_dependencies.h"

#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_fence.h"
#include "vulkan/sync/vulkan_job.h"

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
		LOG_CRITICAL(
			"read dependency does not belong to these job dependencies" );
	}
	else if ( !dependency->active() )
	{
		dependency->active( true );
		state.read_hazards.push_back( dependency );
	}
}

void vulkan_job_dependencies::record_dependencies(
	vulkan_barrier_manager* barrier_manager )
{
	auto& state = job_dependency_state();
	for ( auto& write_dependency : state.write_deps )
	{
		barrier_manager->record_dependency( &write_dependency );
	}

	for ( auto* read_hazard : state.read_hazards )
	{
		barrier_manager->record_dependency( read_hazard );
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


void vulkan_job_dependencies::on_record_cmds(
	const scoped_ptr< vulkan_command_buffer >& command_buffer )
{
	job().on_record_cmds( this );
	on_record_cmds( command_buffer );
}

void vulkan_job_dependencies::resource_reinitialized(
	vulkan_dependency* dependency )
{
	auto& state = job_dependency_state();

	size_t write_index = ( size_t )( dependency - state.write_deps.data() );
	size_t read_index = ( size_t )( dependency - state.read_deps.data() );

	if ( write_index < state.write_deps.size() )
	{
		on_gpu_object_reallocated( dependency );
	}
	else if ( read_index < state.read_deps.size() )
	{
		activate_read_hazard( dependency );
		on_gpu_object_reallocated( dependency );
	}
	else if ( !dependency->active() )
	{
		LOG_CRITICAL( "dependency does not belong to these job dependencies" );
	}
}

void vulkan_job_dependencies::wait_pending_job()
{
	job().wait_on_fence();
}

vulkan_job_dependencies::~vulkan_job_dependencies()
{
	wait_pending_job();
}