
#pragma once

#include "vulkan/batch/vulkan_batch_nodes.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/buffer/vulkan_compute_buffer.h"
#include "vulkan/buffer/vulkan_index_buffer.h"
#include "vulkan/context/vulkan_context.h"
#include "vulkan/shader/vulkan_graphics_pipeline.h"
#include "vulkan/shader/vulkan_pipeline_cache.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_job_attributes.h"
#include "vulkan/sync/vulkan_job_primitives.h"
#include "vulkan/sync/vulkan_job_scope.h"
#include "vulkan/sync/vulkan_poset_fence.h"
#include "vulkan/sync/vulkan_queues.h"
#include "vulkan/texture/vulkan_depth_texture2d.h"
#include "vulkan/texture/vulkan_draw_target.h"
#include "vulkan/texture/vulkan_image.h"
#include "vulkan/texture/vulkan_render_texture2d.h"
#include "vulkan/texture/vulkan_texture2d.h"

#include "igpu/batch/batch_utility.h"
#include "igpu/shader/attribute_indexer.h"

using namespace igpu;

bool vulkan_instance_batch::can_raster(
	vulkan_batch_raster_state* raster_state )
{
	if ( _enabled )
	{
		raster_state->resolved.instance_count =
			(uint32_t)_instance_count.value_or(
				raster_state->fallback.instance_count );

		raster_state->resolved.element_count =
			(uint32_t)_element_count.value_or(
				raster_state->fallback.element_count );

		if ( raster_state->resolved.instance_count &&
			 raster_state->resolved.element_count )
		{
			if ( !_visibility_sphere ||
				 utility::intersects(
					 raster_state->frustum,
					 _visibility_sphere.value() ) )
			{
				raster_state->resolved.base_vertex =
					(int32_t)_base_vertex.value_or(
						raster_state->fallback.base_vertex );
				raster_state->resolved.instance_start =
					(uint32_t)_instance_start.value_or(
						raster_state->fallback.instance_start );
				raster_state->resolved.element_start =
					(uint32_t)_element_start.value_or(
						raster_state->fallback.element_start );

				return true;
			}
		}
	}

	return false;
}

void vulkan_instance_batch::rasterize(
	const vulkan_batch_raster_state& raster_state )
{
	if ( _job_primitives )
	{
		_job_primitives->record_cmds( raster_state.command_buffer );
	}

	vkCmdDrawIndexed(
		raster_state.command_buffer->vk_cmds(),
		raster_state.resolved.element_count,
		raster_state.resolved.instance_count,
		raster_state.resolved.element_start,
		raster_state.resolved.base_vertex,
		raster_state.resolved.instance_start );
}

vulkan_material_batch::vulkan_material_batch( const config& cfg )
	: _cfg( cfg )
{
	if ( cfg.primitives )
	{
		_job_primitives =
			vulkan_job_primitives::make( vulkan_job_primitives::config{
				cfg.root_batch->vk().draw_target->raster_queue(),
				cfg.program->pipeline_layout(),
				cfg.root_batch,
				cfg.root_batch->vk().swap_count,
				1,
				&cfg.program->material_parameters(),
				cfg.primitives.get(),
			} );
	}
}

vulkan_primitives& vulkan_material_batch::item() const
{
	return *_cfg.primitives;
}

vulkan_material_batch::~vulkan_material_batch()
{}

void vulkan_material_batch::start_raster(
	const vulkan_batch_raster_state& raster_state )
{
	if ( _job_primitives )
	{
		_job_primitives->record_cmds( raster_state.command_buffer );
	}
}

void vulkan_material_batch::stop_raster()
{}

vulkan_geometry_batch::vulkan_geometry_batch( const config& cfg )
	: _cfg( cfg )
	, _job_attributes( vulkan_job_attributes::make( {
		  cfg.root_batch->vk().context->cfg().vk.device,
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
				cfg.root_batch->vk().draw_target->raster_queue(),
				cfg.program->pipeline_layout(),
				cfg.root_batch,
				cfg.root_batch->vk().swap_count,
				0,
				&cfg.program->batch_parameters(),
				cfg.batch_primitives.get(),
			} );
	}
}

vulkan_geometry_batch::~vulkan_geometry_batch()
{}

vulkan_geometry& vulkan_geometry_batch::item() const
{
	return *_cfg.geometry;
}

bool vulkan_geometry_batch::pre_raster(
	vulkan_batch_raster_state* raster_state )
{
	const vulkan_geometry& geometry = item();

	if ( !_job_attributes )
	{
		return false;
	}

	raster_state->fallback.instance_start = (uint32_t)geometry.instance_start();
	raster_state->fallback.instance_count = (uint32_t)geometry.instance_count();
	raster_state->fallback.element_start = (uint32_t)geometry.element_start();
	raster_state->fallback.element_count = (uint32_t)geometry.element_count();
	raster_state->fallback.base_vertex = (int32_t)geometry.base_vertex();

	return true;
}

void vulkan_geometry_batch::start_raster(
	const vulkan_batch_raster_state& raster_state )
{
	vkCmdBindPipeline(
		raster_state.command_buffer->vk_cmds(),
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_cfg.pipeline->vk_pipeline() );

	if ( _job_primitives )
	{
		_job_primitives->record_cmds( raster_state.command_buffer );
	}

	_job_attributes->record_cmds( raster_state.command_buffer );
}

void vulkan_geometry_batch::stop_raster()
{}

void vulkan_geometry_batch::rebind_draw_target(
	const scoped_ptr< vulkan_draw_target >& vulkan_draw_target ) const
{
	_cfg.pipeline->rebind_draw_target( vulkan_draw_target );
}

vulkan_states_batch::vulkan_states_batch( const config& cfg )
	: _cfg( cfg )
{}

vulkan_states_batch::~vulkan_states_batch()
{}

vulkan_render_states& vulkan_states_batch::item() const
{
	return *_cfg.states;
}

void vulkan_states_batch::start_raster( const vulkan_batch_raster_state& )
{}

void vulkan_states_batch::stop_raster()
{}

vulkan_program_batch::vulkan_program_batch( const config& cfg )
	: _cfg( cfg )
{}

vulkan_program_batch::~vulkan_program_batch()
{}

vulkan_program& vulkan_program_batch::item() const
{
	return *_cfg.program;
}

void vulkan_program_batch::start_raster( const vulkan_batch_raster_state& )
{}

void vulkan_program_batch::stop_raster()
{}

const vulkan_root_batch::vulkan& vulkan_root_batch::vk() const
{
	return _vk;
}

vulkan_primitives& vulkan_root_batch::item() const
{
	return *_vk.primitives;
}

void vulkan_root_batch::start_raster(
	const vulkan_batch_raster_state& raster_state )
{
	if ( !raster_state.command_buffer )
	{
		LOG_CRITICAL( "command buffer is null" );
	}
	if ( !raster_state.fence )
	{
		LOG_CRITICAL( "fence is null" );
	}
	else if (
		raster_state.command_buffer->queue() !=
		_vk.draw_target->raster_queue() )
	{
		// resources are abandoned to a predetermined queue to streamline
		// destruction, as a result they can only safely be recorded into
		// command buffers that will be submitted to that queue
		LOG_CRITICAL(
			"command buffer does not belong to expected queue unexpected "
			"queue" );
	}
	else
	{
		vulkan_job::fence( raster_state.fence );
	}

	vulkan_job::start_recording_barriers();
}

void vulkan_root_batch::stop_raster()
{
	vulkan_job::submit_recorded_barriers(
		_vk.draw_target->raster_queue(),
		_vk.draw_target->cfg().vk.barrier_manager.get() );
}

void vulkan_root_batch::rebind_draw_target(
	const scoped_ptr< vulkan_draw_target >& vulkan_draw_target )
{
	_vk.draw_target = vulkan_draw_target;

	for ( const auto& program : *this )
	{
		for ( const auto& states : program )
		{
			for ( const vulkan_geometry_batch& geometry : states )
			{
				geometry.rebind_draw_target( vulkan_draw_target );
			}
		}
	}
}

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


	bool valid_args = false;
	if ( !prog )
	{
		LOG_CRITICAL( "program is null" );
	}
	else if ( !states )
	{
		LOG_CRITICAL( "render states is null" );
	}
	else if ( !geo )
	{
		LOG_CRITICAL( "geometry is null" );
	}
	else if ( !mat )
	{
		LOG_CRITICAL( "material parameters is null" );
	}
	else if ( !inst )
	{
		LOG_CRITICAL( "instance parameters is null" );
	}
	else
	{
		valid_args = true;
	}

	if ( !valid_args )
	{
		return nullptr;
	}

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

			auto pipeline =
				_vk.context->make_shared( graphics_pipeline::make_config(
					indexer,
					_vk.draw_target,
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
							   geo,
							   std::move( active_vertex_buffers ),
							   std::dynamic_pointer_cast<
								   vulkan_graphics_pipeline,
								   graphics_pipeline >( pipeline ),
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
	if ( !vk.context )
	{
		LOG_CRITICAL( "context is null" );
	}
	else if ( !vk.draw_target )
	{
		LOG_CRITICAL( "draw_target is null" );
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