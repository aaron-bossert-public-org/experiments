
#include "vulkan/sync/vulkan_job_dependencies.h"

#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_job.h"
#include "vulkan/sync/vulkan_poset_fence.h"

using namespace igpu;

bool vulkan_job_dependencies::is_activated()
{
	auto& state = job_dependency_state();

	return state.staged_transfers.size() || state.read_hazards.size() ||
		state.write_deps.size();
}

void vulkan_job_dependencies::activate_staged_transfer(
	vulkan_dependency* dependency )
{
	auto& state = job_dependency_state();
	auto was_activated = is_activated();

	size_t write_index = ( size_t )( dependency - state.write_deps.data() );
	size_t read_index = ( size_t )( dependency - state.read_deps.data() );

	// use capacity here because we need to be able to activeate staged
	// transfers while we are emplacing dependencies into read_deps and
	// write_deps. also read_deps and write_deps do not get resized after they
	// are initially filled.
	if ( write_index >= state.write_deps.capacity() &&
		 read_index >= state.read_deps.capacity() )
	{
		LOG_CRITICAL(
			"resource dependency does not belong to these job dependencies" );
	}
	else if ( !dependency->is_staged() )
	{
		dependency->is_staged( true );
		state.staged_transfers.push_back( dependency );

		if ( !was_activated )
		{
			job().activate_dependencies( this );
		}
	}
}

void vulkan_job_dependencies::activate_read_hazard(
	vulkan_dependency* dependency )
{
	auto& state = job_dependency_state();
	auto was_activated = is_activated();

	size_t write_index = ( size_t )( dependency - state.write_deps.data() );
	size_t read_index = ( size_t )( dependency - state.read_deps.data() );

	// use capacity here because we need to be able to activeate read hazards
	// while we are emplacing dependencies into read_deps and write_deps. also
	// read_deps and write_deps do not get resized after they are initially
	// filled.
	if ( write_index < state.write_deps.capacity() )
	{
		LOG_CRITICAL( "cannot activate write dependency as read dependency" );
	}
	else if ( read_index >= state.read_deps.capacity() )
	{
		LOG_CRITICAL(
			"read dependency does not belong to these job dependencies" );
	}
	else if ( !dependency->is_hazard() )
	{
		dependency->is_hazard( true );
		state.read_hazards.push_back( dependency );

		if ( !was_activated )
		{
			job().activate_dependencies( this );
		}
	}
}

void vulkan_job_dependencies::record_transfers(
	vulkan_staging_manager* staging_manager )
{
	auto& state = job_dependency_state();
	for ( auto& staged_transfer : state.staged_transfers )
	{
		staging_manager->record_transfer( &staged_transfer->resource() );
	}

	state.staged_transfers.clear();
}

void vulkan_job_dependencies::record_barriers(
	vulkan_barrier_manager* barrier_manager )
{
	auto& state = job_dependency_state();
	for ( auto& write_dependency : state.write_deps )
	{
		barrier_manager->record_barrier(
			&write_dependency.resource(),
			write_dependency.layout(),
			write_dependency.job_scope() );
	}

	for ( auto* read_hazard : state.read_hazards )
	{
		barrier_manager->record_barrier(
			&read_hazard->resource(),
			read_hazard->layout(),
			read_hazard->job_scope() );
		read_hazard->is_hazard( false );
	}

	state.read_hazards.clear();
}

bool vulkan_job_dependencies::validate_hazards() const
{
	auto& state = job_dependency_state();
	bool all_valid = true;

	for ( const vulkan_dependency& dep : state.read_deps )
	{
		if ( false ==
			 dep.resource().validate_hazard( &dep, state.read_hazards ) )
		{
			all_valid = false;
		}
	}

	return all_valid;
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

void vulkan_job_dependencies::resource_reinitialized(
	vulkan_dependency* dependency )
{
	auto& state = job_dependency_state();

	size_t write_index = ( size_t )( dependency - state.write_deps.data() );
	size_t read_index = ( size_t )( dependency - state.read_deps.data() );

	if ( read_index < state.read_deps.size() )
	{
		activate_read_hazard( dependency );
	}
	else if ( write_index >= state.write_deps.size() )
	{
		LOG_CRITICAL( "dependency does not belong to these job dependencies" );
		return;
	}

	on_resource_reinitialized( dependency );
}

void vulkan_job_dependencies::wait_pending_job()
{
	job().wait_on_fence();
}

vulkan_job_dependencies::~vulkan_job_dependencies()
{}