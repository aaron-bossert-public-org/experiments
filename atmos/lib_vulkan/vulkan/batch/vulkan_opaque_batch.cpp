

#include "vulkan/batch/vulkan_opaque_batch.h"

#include "vulkan/batch/vulkan_batch_nodes.h"
#include "vulkan/texture/vulkan_draw_target.h"

using namespace igpu;

const opaque_batch::config& vulkan_opaque_batch::cfg() const
{
	return _cfg;
}

vulkan_opaque_batch::~vulkan_opaque_batch()
{}


void vulkan_opaque_batch::raster(
	const scoped_ptr< draw_target >& draw_target,
	const raster_state& base_raster_state )
{
	auto vulkan_draw_target =
		draw_target.dynamic_ptr_cast< igpu::vulkan_draw_target >();
	if ( draw_target != _cfg.draw_target )
	{
		_cfg.draw_target = draw_target;
		_root_batch->rebind_draw_target( vulkan_draw_target );
	}

	vulkan_batch_raster_state raster_state = {
		base_raster_state,
		vulkan_draw_target->raster_cmds(),
		vulkan_draw_target->raster_fence(),
	};

	batch_utility::raster_opaque( *_root_batch, raster_state );
}


std::unique_ptr< batch_binding > vulkan_opaque_batch::make_binding(
	const instance_batch::config& cfg )
{
	return _root_batch->make_binding( cfg );
}

std::unique_ptr< vulkan_opaque_batch > vulkan_opaque_batch::make(
	const config& cfg )
{
	if ( auto root_batch = vulkan_root_batch::make( cfg.vk ) )
	{
		return std::unique_ptr< vulkan_opaque_batch >(
			new vulkan_opaque_batch( cfg, std::move( root_batch ) ) );
	}

	return nullptr;
}

vulkan_opaque_batch::vulkan_opaque_batch(
	const opaque_batch::config& cfg,
	std::unique_ptr< vulkan_root_batch > root_batch )
	: _cfg( cfg )
	, _root_batch( std::move( root_batch ) )
{}
