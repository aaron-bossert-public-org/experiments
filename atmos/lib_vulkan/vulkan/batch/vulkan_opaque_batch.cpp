

#include "vulkan/batch/vulkan_opaque_batch.h"

#include "vulkan/batch/vulkan_batch_nodes.h"

using namespace igpu;

const vulkan_opaque_batch::config& vulkan_opaque_batch::cfg() const
{
	return _cfg;
}

vulkan_opaque_batch::~vulkan_opaque_batch()
{}

void vulkan_opaque_batch::render( const utility::frustum& frustum )
{
	vulkan_batch_draw_state draw_state = { frustum };
	batch_utility::render_opaque( *_root_batch, draw_state );
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
	const config& cfg,
	std::unique_ptr< vulkan_root_batch > root_batch )
	: _cfg( cfg )
	, _root_batch( std::move( root_batch ) )
{}
