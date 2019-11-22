
#include "vulkan/manager/vulkan_managers.h"


using namespace igpu;

namespace
{}

const vulkan_managers::config& vulkan_managers::cfg() const
{
	return _cfg;
}

vulkan_managers::~vulkan_managers()
{}

std::unique_ptr< vulkan_managers > vulkan_managers::make( const config& cfg )
{
	if ( !cfg.barrier )
	{
		LOG_CRITICAL( "failed to create vulkan_barrier_manager" );
	}
	else if ( !cfg.queues )
	{
		LOG_CRITICAL( "failed to create vulkan_queue_manager" );
	}
	else if ( !cfg.staging )
	{
		LOG_CRITICAL( "failed to create vulkan_staging_manager" );
	}
	else
	{
		return std::unique_ptr< vulkan_managers >( new vulkan_managers( cfg ) );
	}

	return nullptr;
}

vulkan_managers::vulkan_managers( const config& cfg )
	: _cfg( cfg )
{}
