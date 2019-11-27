
#pragma once

#include "vulkan/batch/vulkan_raster_nodes.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/buffer/vulkan_compute_buffer.h"
#include "vulkan/buffer/vulkan_index_buffer.h"
#include "vulkan/context/vulkan_context.h"
#include "vulkan/manager/vulkan_managers.h"
#include "vulkan/shader/vulkan_graphics_pipeline.h"
#include "vulkan/shader/vulkan_pipeline_cache.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_job_attributes.h"
#include "vulkan/sync/vulkan_job_buffers.h"
#include "vulkan/sync/vulkan_job_primitives.h"
#include "vulkan/sync/vulkan_job_scope.h"
#include "vulkan/sync/vulkan_poset_fence.h"
#include "vulkan/texture/vulkan_depth_texture2d.h"
#include "vulkan/texture/vulkan_draw_target.h"
#include "vulkan/texture/vulkan_image.h"
#include "vulkan/texture/vulkan_render_texture2d.h"
#include "vulkan/texture/vulkan_texture2d.h"

#include "igpu/batch/raster_utility.h"
#include "igpu/shader/attribute_indexer.h"

using namespace igpu;

bool vulkan_instance_batch::can_raster( vulkan_raster_state* raster_state )
{
	if ( _enabled )
	{
		if ( std::holds_alternative< draw_indirect_parameters >(
				 _draw_params ) )
		{
			const draw_indirect_parameters& indirect_params =
				std::get< draw_indirect_parameters >( _draw_params );

			return _indirect_draw_dependency && indirect_params.count;
		}
		else
		{
			const draw_parameters* draw_params =
				std::holds_alternative< no_draw_params >( _draw_params )
				? &raster_state->fallback
				: std::holds_alternative< draw_parameters >( _draw_params )
					? &std::get< draw_parameters >( _draw_params )
					: nullptr;


			if ( draw_params )
			{
				return draw_params->index_count && draw_params->instance_count;
			}
			else
			{
				LOG_CRITICAL( "unhandled variant" );
			}
		}
	}

	return false;
}

void vulkan_instance_batch::rasterize( const vulkan_raster_state& raster_state )
{
	if ( _job_primitives )
	{
		_job_primitives->record_cmds( raster_state.command_buffer );
	}

	if ( std::holds_alternative< draw_indirect_parameters >( _draw_params ) )
	{
		const draw_indirect_parameters& indirect_params =
			std::get< draw_indirect_parameters >( _draw_params );

		auto buffer =
			ASSERT_CAST( vulkan_compute_buffer*, indirect_params.buffer.get() );

		_indirect_draw_dependency->record_cmds( raster_state.command_buffer );

		if ( _raster_batch_root->vk()
				 .managers->cfg()
				 .device_features.multiDrawIndirect )
		{
			vkCmdDrawIndexedIndirect(
				raster_state.command_buffer->vk_cmds(),
				buffer->gpu_object().vk_buffer(),
				indirect_params.offset,
				indirect_params.count,
				indirect_params.stride );
		}
		else
		{
			// If multi draw is not available, we must issue separate draw
			// commands
			for ( uint32_t j = 0; j < indirect_params.count; j++ )
			{
				vkCmdDrawIndexedIndirect(
					raster_state.command_buffer->vk_cmds(),
					buffer->gpu_object().vk_buffer(),
					j * indirect_params.stride + indirect_params.offset,
					1,
					indirect_params.stride );
			}
		}
	}
	else
	{
		const draw_parameters* draw_params =
			std::holds_alternative< no_draw_params >( _draw_params )
			? &raster_state.fallback
			: std::holds_alternative< draw_parameters >( _draw_params )
				? &std::get< draw_parameters >( _draw_params )
				: nullptr;


		if ( draw_params )
		{
			vkCmdDrawIndexed(
				raster_state.command_buffer->vk_cmds(),
				draw_params->index_count,
				draw_params->instance_count,
				draw_params->first_index,
				draw_params->base_vertex,
				draw_params->first_instance );
		}
	}
}

vulkan_material_batch::vulkan_material_batch( const config& cfg )
	: _cfg( cfg )
{
	const auto& parameters = cfg.program->material_parameters();
	if ( parameters.count() )
	{
		if ( !cfg.primitives )
		{
			LOG_CRITICAL(
				"program %s expects material parameters but none were provided",
				cfg.program->cfg().name.c_str() );
		}
		else
		{
			_job_primitives =
				vulkan_job_primitives::make( vulkan_job_primitives::config{
					cfg.raster_batch_root->vk().draw_target->raster_queue(),
					cfg.program->pipeline_layout(),
					cfg.raster_batch_root,
					cfg.raster_batch_root->vk().swap_count,
					1,
					&parameters,
					cfg.primitives.get(),
				} );
		}
	}
}

vulkan_primitives& vulkan_material_batch::item() const
{
	return *_cfg.primitives;
}

vulkan_material_batch::~vulkan_material_batch()
{}

void vulkan_material_batch::start_raster(
	const vulkan_raster_state& raster_state )
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
		  cfg.raster_batch_root->vk().context->cfg().vk.device,
		  cfg.raster_batch_root,
		  cfg.raster_batch_root->vk().swap_count,
		  cfg.active_vertex_buffers,
		  cfg.geometry.get(),
	  } ) )
{
	const auto& parameters = cfg.program->batch_parameters();
	if ( parameters.count() )
	{
		if ( !cfg.batch_primitives )
		{
			LOG_CRITICAL(
				"program %s expects batch parameters but none were provided",
				cfg.program->cfg().name.c_str() );
		}
		else
		{
			_job_primitives =
				vulkan_job_primitives::make( vulkan_job_primitives::config{
					cfg.raster_batch_root->vk().draw_target->raster_queue(),
					cfg.program->pipeline_layout(),
					cfg.raster_batch_root,
					cfg.raster_batch_root->vk().swap_count,
					0,
					&parameters,
					cfg.batch_primitives.get(),
				} );
		}
	}
}

vulkan_geometry_batch::~vulkan_geometry_batch()
{}

vulkan_geometry& vulkan_geometry_batch::item() const
{
	return *_cfg.geometry;
}

bool vulkan_geometry_batch::pre_raster( vulkan_raster_state* raster_state )
{
	const vulkan_geometry& geometry = item();

	if ( !_job_attributes )
	{
		return false;
	}

	raster_state->fallback = geometry.draw_parameters();

	return true;
}

void vulkan_geometry_batch::start_raster(
	const vulkan_raster_state& raster_state )
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

void vulkan_states_batch::start_raster( const vulkan_raster_state& )
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

void vulkan_program_batch::start_raster( const vulkan_raster_state& )
{}

void vulkan_program_batch::stop_raster()
{}

const vulkan_raster_batch_root::vulkan& vulkan_raster_batch_root::vk() const
{
	return _vk;
}

vulkan_primitives& vulkan_raster_batch_root::item() const
{
	return *_vk.primitives;
}

void vulkan_raster_batch_root::start_raster(
	const vulkan_raster_state& raster_state )
{
	if ( !raster_state.command_buffer )
	{
		LOG_CRITICAL( "command buffer is null" );
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
		vulkan_job::fence( vulkan_poset_fence::current(
			_vk.draw_target->raster_queue().get() ) );
	}

	vulkan_job::submit_activated_dependency_barriers(
		_vk.draw_target->raster_queue(),
		_vk.draw_target->cfg().vk.managers.get() );
}

void vulkan_raster_batch_root::stop_raster()
{}

void vulkan_raster_batch_root::rebind_draw_target(
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

std::unique_ptr< vulkan_raster_binding > vulkan_raster_batch_root::make_binding(
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
					_vk.constants,
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

	return std::unique_ptr< vulkan_raster_binding >( new vulkan_raster_binding(
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

std::unique_ptr< vulkan_raster_batch_root > vulkan_raster_batch_root::make(
	const vulkan& vk )
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
		return std::unique_ptr< vulkan_raster_batch_root >(
			new vulkan_raster_batch_root( vk ) );
	}

	return nullptr;
}

vulkan_raster_batch_root::vulkan_raster_batch_root( const vulkan& vk )
	: _vk( vk )
{}

vulkan_raster_batch_root::~vulkan_raster_batch_root()
{}

vulkan_job::state& vulkan_raster_batch_root::job_state()
{
	return _job_state;
}

const vulkan_job::state& vulkan_raster_batch_root::job_state() const
{
	return _job_state;
}

vulkan_raster_binding::~vulkan_raster_binding()
{}