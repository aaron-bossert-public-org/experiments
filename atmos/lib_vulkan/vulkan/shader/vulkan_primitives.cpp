
#include "vulkan/shader/vulkan_primitives.h"

#include "vulkan/buffer/vulkan_compute_buffer.h"
#include "vulkan/buffer/vulkan_index_buffer.h"
#include "vulkan/buffer/vulkan_vertex_buffer.h"
#include "vulkan/texture/vulkan_depth_texture2d.h"
#include "vulkan/texture/vulkan_render_texture2d.h"
#include "vulkan/texture/vulkan_texture2d.h"

using namespace igpu;

namespace
{
	std::shared_ptr< vulkan_compute_buffer > to_vulkan_variant(
		const std::shared_ptr< compute_buffer >& ptr )
	{
		return std::
			dynamic_pointer_cast< vulkan_compute_buffer, compute_buffer >(
				ptr );
	}

	std::shared_ptr< vulkan_vertex_buffer > to_vulkan_variant(
		const std::shared_ptr< vertex_buffer >& ptr )
	{
		return std::dynamic_pointer_cast< vulkan_vertex_buffer, vertex_buffer >(
			ptr );
	}

	std::shared_ptr< vulkan_index_buffer > to_vulkan_variant(
		const std::shared_ptr< index_buffer >& ptr )
	{
		return std::dynamic_pointer_cast< vulkan_index_buffer, index_buffer >(
			ptr );
	}

	std::shared_ptr< vulkan_texture2d > to_vulkan_variant(
		const std::shared_ptr< texture2d >& ptr )
	{
		return std::dynamic_pointer_cast< vulkan_texture2d, texture2d >( ptr );
	}

	std::shared_ptr< vulkan_depth_texture2d > to_vulkan_variant(
		const std::shared_ptr< depth_texture2d >& ptr )
	{
		return std::
			dynamic_pointer_cast< vulkan_depth_texture2d, depth_texture2d >(
				ptr );
	}

	std::shared_ptr< vulkan_render_texture2d > to_vulkan_variant(
		const std::shared_ptr< render_texture2d >& ptr )
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
	primitives.reserve( cfg.primitives.size() );

	for ( const auto& prim_cfg : cfg.primitives )
	{
		std::visit(
			[&]( auto&& ptr ) {
				auto vulkan = to_vulkan_variant( ptr );
				vulkan_primitive::config vulkan_cfg = {
					prim_cfg,
					&vulkan->gpu_object(),
					vulkan,
				};
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