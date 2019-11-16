

#include "vulkan/batch/vulkan_transparent_batch.h"

#include "vulkan/batch/vulkan_batch_nodes.h"

using namespace igpu;

const vulkan_transparent_batch::config& vulkan_transparent_batch::cfg() const
{
	return _cfg;
}

vulkan_transparent_batch::~vulkan_transparent_batch()
{}

void vulkan_transparent_batch::raster(
	const scoped_ptr< draw_target >&,
	const raster_state& )
{
	LOG_CRITICAL( "not implemented" );
	// batch_utility::render_transparent(*_root_batch, frustum);
}

std::unique_ptr< batch_binding > vulkan_transparent_batch::make_binding(
	const instance_batch::config& cfg )
{
	return _root_batch->make_binding( cfg );
}

std::unique_ptr< vulkan_transparent_batch > vulkan_transparent_batch::make(
	const config& cfg )
{
	if ( auto root_batch = vulkan_root_batch::make( cfg.vk ) )
	{
		return std::unique_ptr< vulkan_transparent_batch >(
			new vulkan_transparent_batch( cfg, std::move( root_batch ) ) );
	}

	return nullptr;
}

vulkan_transparent_batch::vulkan_transparent_batch(
	const config& cfg,
	std::unique_ptr< vulkan_root_batch > root_batch )
	: _cfg( cfg )
	, _root_batch( std::move( root_batch ) )
{}
