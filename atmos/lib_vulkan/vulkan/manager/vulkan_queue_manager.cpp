
#include "vulkan/manager/vulkan_queue_manager.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/manager/vulkan_abandon_manager.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_poset_fence.h"
#include "vulkan/sync/vulkan_queue.h"
#include "vulkan/texture/vulkan_image.h"

#include < algorithm>

using namespace igpu;

namespace
{}

const vulkan_queue_manager::config& vulkan_queue_manager::cfg() const
{
	return _cfg;
}

vulkan_queue_manager::~vulkan_queue_manager()
{}

std::unique_ptr< vulkan_queue_manager > vulkan_queue_manager::make(
	const config& cfg )
{
	if ( !cfg.present_queue )
	{
		LOG_CRITICAL( "failed to create present_queue" );
	}
	else if ( !cfg.graphics_queue )
	{
		LOG_CRITICAL( "failed to create graphics_queue" );
	}
	else if ( !cfg.compute_queue )
	{
		LOG_CRITICAL( "failed to create compute_queue" );
	}
	else if ( !cfg.transfer_queue )
	{
		LOG_CRITICAL( "failed to create transfer_queue" );
	}
	else
	{
		return std::unique_ptr< vulkan_queue_manager >(
			new vulkan_queue_manager( cfg ) );
	}

	return nullptr;
}

vulkan_queue_manager::vulkan_queue_manager( const config& cfg )
	: _cfg( cfg )
{}
