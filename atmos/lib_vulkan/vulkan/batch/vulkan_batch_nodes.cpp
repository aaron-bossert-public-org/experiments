
#pragma once

#include "vulkan/batch/vulkan_batch_nodes.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/buffer/vulkan_compute_buffer.h"
#include "vulkan/buffer/vulkan_index_buffer.h"
#include "vulkan/context/vulkan_context.h"
#include "vulkan/sync/vulkan_fence.h"
#include "vulkan/texture/vulkan_depth_texture2d.h"
#include "vulkan/texture/vulkan_image.h"
#include "vulkan/texture/vulkan_render_texture2d.h"
#include "vulkan/texture/vulkan_texture2d.h"

#include "igpu/batch/batch_utility.h"

using namespace igpu;

bool vulkan_instance_batch::can_render( vulkan_batch_draw_state* draw_state )
{
	if ( _enabled )
	{
		draw_state->resolved.instance_count =
			(uint32_t)_instance_count.value_or(
				draw_state->fallback.instance_count );
		draw_state->resolved.element_count = (uint32_t)_element_count.value_or(
			draw_state->fallback.element_count );

		if ( draw_state->resolved.instance_count &&
			 draw_state->resolved.element_count )
		{
			if ( !_visibility_sphere ||
				 utility::intersects(
					 draw_state->frustum,
					 _visibility_sphere.value() ) )
			{
				draw_state->resolved.base_vertex =
					(int32_t)_base_vertex.value_or(
						draw_state->fallback.base_vertex );
				draw_state->resolved.instance_start =
					(uint32_t)_instance_start.value_or(
						draw_state->fallback.instance_start );
				draw_state->resolved.element_start =
					(uint32_t)_element_start.value_or(
						draw_state->fallback.element_start );

				return true;
			}
		}
	}

	return false;
}

void vulkan_instance_batch::draw( const vulkan_batch_draw_state& draw_state )
{
	vkCmdDrawIndexed(
		draw_state.command_buffer,
		draw_state.resolved.element_count,
		draw_state.resolved.instance_count,
		draw_state.resolved.element_start,
		draw_state.resolved.base_vertex,
		draw_state.resolved.instance_start );
}

vulkan_geometry_batch::vulkan_geometry_batch(
	const vulkan_instance_batch::config& cfg )
	: _root_batch( cfg.vk.root_batch )
{
	vulkan_program* program = ASSERT_CAST( vulkan_program*, cfg.program.get() );
	vulkan_attribute_finder attribute_finder;
	if ( attribute_finder.find_all_attributes( *program, *cfg.vk.geometry ) )
	{
		_active_buffer_count = attribute_finder.binding_description_count();
		for ( size_t i = 0; i < _active_buffer_count; ++i )
		{
			_active_buffers[i] =
				(uint8_t)attribute_finder.binding_descriptions()[i].binding;
		}
	}
}

vulkan_geometry_batch::~vulkan_geometry_batch()
{
	auto fence = _root_batch->fence();
	if ( fence )
	{
		item().add_fence( fence );
	}
}

void vulkan_geometry_batch::pre_draw( vulkan_batch_draw_state* draw_state )
{
	const vulkan_geometry& geometry = item();

	draw_state->fallback.base_vertex = (int32_t)geometry.base_vertex();
	draw_state->fallback.instance_start = (uint32_t)geometry.instance_start();
	draw_state->fallback.instance_count = (uint32_t)geometry.instance_count();
	draw_state->fallback.element_start = (uint32_t)geometry.element_start();
	draw_state->fallback.element_count = (uint32_t)geometry.element_count();
}

void vulkan_geometry_batch::start_draw(
	const vulkan_batch_draw_state& draw_state )
{
	const vulkan_geometry& geometry = item();
	const vulkan_index_buffer& index_buffer = geometry.index_buffer();
	const auto& geo_byte_offsets = geometry.cfg().vbuff_byte_offsets;

	VkBuffer vk_buffers[16];
	VkDeviceSize ibuff_byte_offset =
		(VkDeviceSize)geometry.cfg().ibuff_byte_offset;
	VkDeviceSize vbuff_byte_offsets[16];

	for ( size_t i = 0; i < _active_buffer_count; ++i )
	{
		size_t active_buffer = _active_buffers[i];
		vk_buffers[i] =
			geometry.vertex_buffer( active_buffer ).gpu_object().get();
		vbuff_byte_offsets[i] =
			i < geo_byte_offsets.size() ? geo_byte_offsets[i] : 0;
	}

	// should only need to bind vertex buffers in use by program here,
	// will require adjusting vulkan_vertex_mapper's logic to produce
	vkCmdBindVertexBuffers(
		draw_state.command_buffer,
		0,
		_active_buffer_count,
		vk_buffers,
		vbuff_byte_offsets );

	vkCmdBindIndexBuffer(
		draw_state.command_buffer,
		index_buffer.gpu_object().get(),
		ibuff_byte_offset,
		index_buffer.cfg().vk.format );
}

void vulkan_geometry_batch::stop_draw()
{}

vulkan_geometry* vulkan_geometry_batch::get_key(
	const vulkan_instance_batch::config& cfg )
{
	return cfg.vk.geometry.get();
}

vulkan_material_batch::vulkan_material_batch(
	const vulkan_instance_batch::config& cfg )
	: _root_batch( cfg.vk.root_batch )
{}

vulkan_material_batch::~vulkan_material_batch()
{
	auto fence = _root_batch->fence();
	if ( fence )
	{
		item().visit(
			[&fence]( auto&& ptr ) { ptr->gpu_object().add_fence( fence ); } );
	}
}

void vulkan_material_batch::start_draw( const vulkan_batch_draw_state& )
{}

void vulkan_material_batch::stop_draw()
{}

vulkan_primitives* vulkan_material_batch::get_key(
	const vulkan_instance_batch::config& cfg )
{
	return cfg.vk.material.get();
}

vulkan_graphics_pipeline_batch::vulkan_graphics_pipeline_batch(
	const vulkan_instance_batch::config& cfg )
	: _root_batch( cfg.vk.root_batch )
{}

vulkan_graphics_pipeline_batch::~vulkan_graphics_pipeline_batch()
{
	auto fence = _root_batch->fence();
	if ( fence )
	{
		item().add_fence( fence );
	}
}

void vulkan_graphics_pipeline_batch::start_draw(
	const vulkan_batch_draw_state& )
{}

void vulkan_graphics_pipeline_batch::stop_draw()
{}

vulkan_graphics_pipeline* vulkan_graphics_pipeline_batch::get_key(
	const vulkan_instance_batch::config& cfg )
{
	return cfg.vk.graphics_pipeline.get();
}


vulkan_program_batch::vulkan_program_batch(
	const vulkan_instance_batch::config& cfg )
	: _root_batch( cfg.vk.root_batch )
{}

vulkan_program_batch::~vulkan_program_batch()
{}

void vulkan_program_batch::start_draw( const vulkan_batch_draw_state& )
{}

void vulkan_program_batch::stop_draw()
{}

vulkan_program* vulkan_program_batch::get_key(
	const vulkan_instance_batch::config& cfg )
{
	return ASSERT_CAST( vulkan_program*, cfg.program.get() );
}

const vulkan_root_batch::config& vulkan_root_batch::cfg() const
{
	return _cfg;
}

void vulkan_root_batch::start_draw( const vulkan_batch_draw_state& draw_state )
{
	_fence = draw_state.fence;
	ASSERT_CONTEXT( (bool)_fence );
}

void vulkan_root_batch::stop_draw()
{}

const std::shared_ptr< vulkan_fence >& vulkan_root_batch::fence() const
{
	return _fence;
}

std::unique_ptr< vulkan_batch_binding > vulkan_root_batch::make_binding(
	const instance_batch::config& base_cfg )
{
	vulkan_instance_batch::config cfg{
		base_cfg,
		{
			this,
			/*memoize_pipeline(
				base_cfg.program,
				base_cfg.render_states,
				base_cfg.geometry ),
			std::dynamic_pointer_cast< vulkan_primitives, primitives >(
				cfg.material ),
			std::dynamic_pointer_cast< vulkan_geometry, geometry >(
				cfg.geometry ),
			std::dynamic_pointer_cast< vulkan_primitives, primitives >(
				cfg.primitives ),*/
		},
	};

	return batch_utility::make_binding< vulkan_batch_binding >( *this, cfg );
}

std::unique_ptr< vulkan_root_batch > vulkan_root_batch::make(
	const config& cfg )
{
	if ( !cfg.context )
	{
		LOG_CRITICAL( "context is null" );
	}
	else
	{
		return std::unique_ptr< vulkan_root_batch >(
			new vulkan_root_batch( cfg ) );
	}

	return nullptr;
}

vulkan_root_batch::vulkan_root_batch( const config& cfg )
	: _cfg( cfg )
{}

vulkan_root_batch::~vulkan_root_batch()
{
	if ( _fence )
	{
		_fence->wait();
	}
}

vulkan_batch_binding::~vulkan_batch_binding()
{
	const auto& cfg = this->cfg();
	auto fence = cfg.vk.root_batch->fence();
	if ( fence )
	{
		cfg.vk.primitives->visit(
			[&fence]( auto&& ptr ) { ptr->gpu_object().add_fence( fence ); } );
	}
}