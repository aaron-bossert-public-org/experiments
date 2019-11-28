
#include "vulkan/shader/vulkan_primitives.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/buffer/vulkan_index_buffer.h"
#include "vulkan/buffer/vulkan_vertex_buffer.h"
#include "vulkan/compute/vulkan_compute_buffer.h"
#include "vulkan/texture/vulkan_depth_texture2d.h"
#include "vulkan/texture/vulkan_image.h"
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

	bool validate( const char* name, const vulkan_buffer& buffer )
	{
		if ( !buffer.vk_buffer() )
		{
			LOG_CRITICAL( "%s is empty", name );
			return false;
		}

		return true;
	}

	bool validate( const char* name, const vulkan_image& image )
	{
		if ( !image.vk_image_view() )
		{
			LOG_CRITICAL( "%s is empty", name );
			return false;
		}

		return true;
	}
}

const vulkan_primitives::config& vulkan_primitives::cfg() const
{
	return _cfg;
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

	bool success = true;

	for ( const auto& prim_cfg : cfg )
	{
		std::visit(
			[&]( auto&& ptr ) {
				auto vulkan = to_vulkan_variant( ptr );

				if ( !vulkan )
				{
					success = false;
					LOG_CRITICAL( "%s is null", prim_cfg.name.c_str() );
				}
				else if ( !validate(
							  prim_cfg.name.c_str(),
							  vulkan->gpu_object() ) )
				{
					success = false;
				}
				else
				{
					vulkan_primitive::config vulkan_cfg = {
						prim_cfg,
						&vulkan->gpu_object(),
						vulkan,
					};
					primitives.emplace_back( vulkan_cfg );
				}
			},
			prim_cfg.value );
	}

	if ( success )
	{
		return std::unique_ptr< vulkan_primitives >(
			new vulkan_primitives( cfg, std::move( primitives ) ) );
	}

	return nullptr;
}

vulkan_primitives::vulkan_primitives(
	const config& cfg,
	std::vector< vulkan_primitive > primitives )
	: _cfg( cfg )
	, _primitives( std::move( primitives ) )
{}