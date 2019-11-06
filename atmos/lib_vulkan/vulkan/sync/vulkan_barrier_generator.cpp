
#include "vulkan/sync/vulkan_barrier_generator.h"

#include "vulkan/sync/vulkan_barrier.h"

#include "framework/logging/log.h"

using namespace igpu;

void vulkan_barrier_generator::activate_read_barrier(
	vulkan_dependency* read_dependency )
{
#if ATMOS_DEBUG
	if ( is_write_dependency( *read_dependency ) )
	{
		LOG_CRITICAL(
			"attempting to activate write dependency as read dependency" );
	}
#endif

	if ( read_dependency->active() )
	{
		LOG_CRITICAL(
			"attempting to activate read dependency that is already "
			"active" );
	}
	else
	{
		read_dependency->active( true );
		_read_dependencies.push_back( read_dependency );
	}
}

void vulkan_barrier_generator::generate_barriers(
	vulkan_barrier_cluster* cluster )
{
	for ( auto& write_dependency : _write_dependencies )
	{
		cluster->update_write_dependency( &write_dependency );
		write_dependency.reasource().activate_read_barriers();
	}

	for ( auto* read_dependency : _read_dependencies )
	{
		if ( read_barrier_required( read_dependency ) )
		{
			if ( layout_required( read_dependency ) )
			{
				cluster->update_read_dependency_layout( read_dependency );
				read_dependency->resource().activate_read_barriers();
			}
			else
			{
				cluster->update_read_dependency( read_dependency );
			}
		}

		read_dependency->active( false );
	}

	_read_dependencies.clear();
}