
#pragma once

#include "vulkan/batch/vulkan_batch_nodes.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/buffer/vulkan_compute_buffer.h"
#include "vulkan/buffer/vulkan_index_buffer.h"
#include "vulkan/context/vulkan_context.h"
#include "vulkan/shader/vulkan_pipeline_cache.h"
#include "vulkan/sync/vulkan_fence.h"
#include "vulkan/sync/vulkan_job_attributes.h"
#include "vulkan/sync/vulkan_job_primitives.h"
#include "vulkan/sync/vulkan_job_scope.h"
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
	, _job_attributes( vulkan_job_attributes::make( {
		  cfg.vk.root_batch->vk().device,
		  cfg.vk.root_batch,
		  cfg.vk.root_batch->vk().swap_count,
		  cfg.vk.vertex_buffer_indices,
		  cfg.vk.geometry.get(),
	  } ) )
{}

vulkan_geometry_batch::~vulkan_geometry_batch()
{}

bool vulkan_geometry_batch::pre_draw( vulkan_batch_draw_state* draw_state )
{
	const vulkan_geometry& geometry = item();

	if ( !_job_attributes )
	{
		return false;
	}

	draw_state->fallback.instance_start = (uint32_t)geometry.instance_start();
	draw_state->fallback.instance_count = (uint32_t)geometry.instance_count();
	draw_state->fallback.element_start = (uint32_t)geometry.element_start();
	draw_state->fallback.element_count = (uint32_t)geometry.element_count();
	draw_state->fallback.base_vertex = (int32_t)geometry.base_vertex();

	return true;
}

void vulkan_geometry_batch::start_draw(
	const vulkan_batch_draw_state& draw_state )
{
	_job_attributes->bind_buffer_cmds( draw_state.command_buffer );
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
	, _job_primitives( vulkan_job_primitives::make( {
		  cfg.vk.root_batch->vk().device,
		  cfg.vk.root_batch,
		  cfg.vk.root_batch->vk().swap_count,
		  &cfg.vk.program->material_parameters(),
		  cfg.vk.material.get(),
	  } ) )
{}

vulkan_material_batch::~vulkan_material_batch()
{}

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
		fence->wait();
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

const vulkan_root_batch::vulkan& vulkan_root_batch::vk() const
{
	return _vk;
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
	auto prog = std::dynamic_pointer_cast< vulkan_program, program >(
		base_cfg.program );


	auto geo = std::dynamic_pointer_cast< vulkan_geometry, geometry >(
		base_cfg.geometry );

	vulkan_attributes_decriptor attributes_descriptor;
	if ( attributes_descriptor.reset( prog->vertex_parameters(), *geo ) )
	{
		auto mat = std::dynamic_pointer_cast< vulkan_primitives, primitives >(
			base_cfg.material );

		auto prim = std::dynamic_pointer_cast< vulkan_primitives, primitives >(
			base_cfg.material );

		auto pipeline_config =
			graphics_pipeline::make_config( attributes_descriptor.indexer() );

		auto pipeline = _vk.pipeline_cache->memoized( pipeline_config );

		vulkan_instance_batch::config cfg{
			base_cfg,
			this,
			pipeline,
			prog,
			mat,
			geo,
			prim,
		};

		return batch_utility::make_binding< vulkan_batch_binding >(
			*this,
			cfg );
	};


	return nullptr;
}

std::unique_ptr< vulkan_root_batch > vulkan_root_batch::make( const vulkan& vk )
{
	if ( !vk.device )
	{
		LOG_CRITICAL( "device is null" );
	}
	else if ( !vk.swap_count )
	{
		LOG_CRITICAL( "swap_count is zero" );
	}
	else if ( !vk.pipeline_cache )
	{
		LOG_CRITICAL( "pipeline_cache is null" );
	}
	else
	{
		return std::unique_ptr< vulkan_root_batch >(
			new vulkan_root_batch( vk ) );
	}

	return nullptr;
}

vulkan_root_batch::vulkan_root_batch( const vulkan& vk )
	: _vk( vk )
{}

vulkan_root_batch::~vulkan_root_batch()
{
	if ( _fence )
	{
		_fence->wait();
	}
}

vulkan_batch_binding::~vulkan_batch_binding()
{}