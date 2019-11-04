
#include "vulkan/shader/vulkan_primitives.h"
#include "vulkan/buffer/vulkan_compute_buffer.h"
#include "vulkan/texture/vulkan_depth_texture2d.h"
#include "vulkan/texture/vulkan_render_texture2d.h"
#include "vulkan/texture/vulkan_texture2d.h"

using namespace igpu;

namespace
{
	vulkan_primitive::vulkan_variant_t to_vulkan_variant(
		const std::shared_ptr< compute_buffer > ptr )
	{
		return std::
			dynamic_pointer_cast< vulkan_compute_buffer, compute_buffer >(
				ptr );
	}

	vulkan_primitive::vulkan_variant_t to_vulkan_variant(
		const std::shared_ptr< texture2d > ptr )
	{
		return std::dynamic_pointer_cast< vulkan_texture2d, texture2d >( ptr );
	}

	vulkan_primitive::vulkan_variant_t to_vulkan_variant(
		const std::shared_ptr< depth_texture2d > ptr )
	{
		return std::
			dynamic_pointer_cast< vulkan_depth_texture2d, depth_texture2d >(
				ptr );
	}

	vulkan_primitive::vulkan_variant_t to_vulkan_variant(
		const std::shared_ptr< render_texture2d > ptr )
	{
		return std::
			dynamic_pointer_cast< vulkan_render_texture2d, render_texture2d >(
				ptr );
	}
}

size_t vulkan_primitives::primitive_count() const
{
	return _primitives.size();
}

const vulkan_primitive& vulkan_primitives::primitive( size_t i ) const
{
	return _primitives[i];
}

const vulkan_primitive* vulkan_primitives::begin() const
{
	return &_primitives[0];
}

const vulkan_primitive* vulkan_primitives::end() const
{
	return begin() + _primitives.size();
}

std::unique_ptr< vulkan_primitives > vulkan_primitives::make(
	const config& cfg )
{
	std::vector< vulkan_primitive > primitives;
	primitives.reserve( cfg.size() );

	for ( const auto& prim_cfg : cfg )
	{
		std::visit(
			[&]( auto&& ptr ) {
				vulkan_primitive::config vulkan_cfg = {
					prim_cfg,
					to_vulkan_variant( ptr ) };
				primitives.emplace_back( vulkan_cfg );
			},
			prim_cfg.value );
	}

	return std::unique_ptr< vulkan_primitives >(
		new vulkan_primitives( std::move( primitives ) ) );
}

vulkan_primitives::vulkan_primitives(
	std::vector< vulkan_primitive > primitives )
	: _primitives( std::move( primitives ) )
{}