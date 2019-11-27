

#include "vulkan/batch/vulkan_transparent_batch.h"

#include "vulkan/batch/vulkan_raster_nodes.h"

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
	// raster_utility::render_transparent(*_raster_batch_root, frustum);
}

std::unique_ptr< raster_binding > vulkan_transparent_batch::make_binding(
	const instance_batch::config& cfg )
{
	return _raster_batch_root->make_binding( cfg );
}

std::unique_ptr< vulkan_transparent_batch > vulkan_transparent_batch::make(
	const config& cfg )
{
	if ( auto raster_batch_root = vulkan_raster_batch_root::make( cfg.vk ) )
	{
		return std::unique_ptr< vulkan_transparent_batch >(
			new vulkan_transparent_batch(
				cfg,
				std::move( raster_batch_root ) ) );
	}

	return nullptr;
}

vulkan_transparent_batch::vulkan_transparent_batch(
	const config& cfg,
	std::unique_ptr< vulkan_raster_batch_root > raster_batch_root )
	: _cfg( cfg )
	, _raster_batch_root( std::move( raster_batch_root ) )
{}
