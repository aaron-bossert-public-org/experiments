
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
#include "igpu/shader/attribute_indexer.h"

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
	if ( _job_primitives )
	{
		_job_primitives->record_cmds( draw_state.command_buffer );
	}

	vkCmdDrawIndexed(
		draw_state.command_buffer,
		draw_state.resolved.element_count,
		draw_state.resolved.instance_count,
		draw_state.resolved.element_start,
		draw_state.resolved.base_vertex,
		draw_state.resolved.instance_start );
}

vulkan_material_batch::vulkan_material_batch( const config& cfg )
	: _cfg( cfg )
{
	if ( cfg.primitives )
	{
		_job_primitives =
			vulkan_job_primitives::make( vulkan_job_primitives::config{
				cfg.root_batch->vk().device,
				cfg.program->pipeline_layout(),
				cfg.root_batch,
				cfg.root_batch->vk().swap_count,
				1,
				&cfg.program->material_parameters(),
				cfg.primitives.get(),
			} );
	}
}

vulkan_material_batch::~vulkan_material_batch()
{}

void vulkan_material_batch::start_draw(
	const vulkan_batch_draw_state& draw_state )
{
	if ( _job_primitives )
	{
		_job_primitives->record_cmds( draw_state.command_buffer );
	}
}

void vulkan_material_batch::stop_draw()
{}

vulkan_geometry_batch::vulkan_geometry_batch( const config& cfg )
	: _cfg( cfg )
	, _job_attributes( vulkan_job_attributes::make( {
		  cfg.root_batch->vk().device,
		  cfg.root_batch,
		  cfg.root_batch->vk().swap_count,
		  cfg.active_vertex_buffers,
		  cfg.geometry.get(),
	  } ) )
{
	if ( cfg.batch_primitives )
	{
		_job_primitives =
			vulkan_job_primitives::make( vulkan_job_primitives::config{
				cfg.root_batch->vk().device,
				cfg.program->pipeline_layout(),
				cfg.root_batch,
				cfg.root_batch->vk().swap_count,
				0,
				&cfg.program->material_parameters(),
				cfg.batch_primitives.get(),
			} );
	}
}

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
	vkCmdBindPipeline(
		draw_state.command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_cfg.pipeline->vk_pipeline() );

	if ( _job_primitives )
	{
		_job_primitives->record_cmds( draw_state.command_buffer );
	}

	_job_attributes->record_cmds( draw_state.command_buffer );
}

void vulkan_geometry_batch::stop_draw()
{}

vulkan_states_batch::vulkan_states_batch( const config& cfg )
	: _cfg( cfg )
{}

vulkan_states_batch::~vulkan_states_batch()
{}

void vulkan_states_batch::start_draw( const vulkan_batch_draw_state& )
{}

void vulkan_states_batch::stop_draw()
{}

vulkan_program_batch::vulkan_program_batch( const config& cfg )
	: _cfg( cfg )
{}

vulkan_program_batch::~vulkan_program_batch()
{}

void vulkan_program_batch::start_draw( const vulkan_batch_draw_state& )
{}

void vulkan_program_batch::stop_draw()
{}

const vulkan_root_batch::vulkan& vulkan_root_batch::vk() const
{
	return _vk;
}

void vulkan_root_batch::start_draw( const vulkan_batch_draw_state& draw_state )
{
	ASSERT_CONTEXT( (bool)draw_state.fence );
	vulkan_job::fence( draw_state.fence );
}

void vulkan_root_batch::stop_draw()
{}

std::unique_ptr< vulkan_batch_binding > vulkan_root_batch::make_binding(
	const instance_batch::config& cfg )
{
	auto prog =
		std::dynamic_pointer_cast< vulkan_program, program >( cfg.program );

	auto states =
		std::dynamic_pointer_cast< vulkan_render_states, render_states >(
			cfg.states );

	auto geo =
		std::dynamic_pointer_cast< vulkan_geometry, geometry >( cfg.geometry );

	auto mat = std::dynamic_pointer_cast< vulkan_primitives, primitives >(
		cfg.material );

	auto inst = std::dynamic_pointer_cast< vulkan_primitives, primitives >(
		cfg.instance );

	vulkan_program_batch* program_batch = nullptr;
	{
		auto& map = this->map();
		auto found = map.find( prog.get() );
		if ( !map.found( found ) )
		{
			found = map.emplace(
						   prog.get(),
						   vulkan_program_batch::config{
							   this,
							   prog,
						   } )
						.first;
		}
		program_batch = &found->second->val();
	}

	vulkan_states_batch* states_batch = nullptr;
	{
		auto& map = program_batch->map();
		auto found = map.find( states.get() );
		if ( !map.found( found ) )
		{
			found = map.emplace(
						   states.get(),
						   vulkan_states_batch::config{
							   this,
							   states,
						   } )
						.first;
		}
		states_batch = &found->second->val();
	}

	vulkan_geometry_batch* geometry_batch = nullptr;
	{
		auto& map = states_batch->map();
		auto found = map.find( geo.get() );
		if ( !map.found( found ) )
		{
			attribute_indexer indexer;
			if ( !indexer.reset( prog->vertex_parameters(), *geo ) )
			{
				LOG_CRITICAL(
					"failed to index vertex attributes for batch binding" );
				return nullptr;
			}

			auto pipeline = _vk.pipeline_cache->memoized(
				indexer,
				graphics_pipeline::make_config(
					indexer,
					cfg.program,
					cfg.states ) );

			const auto& buffer_indices = indexer.buffer_indices();

			std::vector< size_t > active_vertex_buffers(
				buffer_indices.data(),
				buffer_indices.data() + indexer.buffer_count() );

			found = map.emplace(
						   geo.get(),
						   vulkan_geometry_batch::config{
							   this,
							   prog,
							   _vk.primitives,
							   pipeline,
							   geo,
							   std::move( active_vertex_buffers ),
						   } )
						.first;
		}
		geometry_batch = &found->second->val();
	}

	vulkan_material_batch* material_batch = nullptr;
	{
		auto& map = geometry_batch->map();
		auto found = map.find( mat.get() );
		if ( !map.found( found ) )
		{
			found = map.emplace(
						   mat.get(),
						   vulkan_material_batch::config{
							   this,
							   prog,
							   mat,
						   } )
						.first;
		}
		material_batch = &found->second->val();
	}

	return std::unique_ptr< vulkan_batch_binding >( new vulkan_batch_binding(
		this,
		program_batch,
		states_batch,
		geometry_batch,
		material_batch,
		{
			cfg,
			this,
			prog,
			inst,
		} ) );
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
	else if ( !vk.primitives )
	{
		LOG_CRITICAL( "primitives is null" );
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
	vulkan_job::wait_on_fence();
}

vulkan_job::state& vulkan_root_batch::job_state()
{
	return _job_state;
}

const vulkan_job::state& vulkan_root_batch::job_state() const
{
	return _job_state;
}

vulkan_batch_binding::~vulkan_batch_binding()
{}