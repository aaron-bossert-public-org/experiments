
#pragma once


#include "vulkan/batch/vulkan_instance_batch.h"

#include "vulkan/batch/vulkan_batch_nodes.h"
#include "vulkan/manager/vulkan_managers.h"
#include "vulkan/manager/vulkan_queue_manager.h"
#include "vulkan/sync/vulkan_job_buffers.h"
#include "vulkan/sync/vulkan_job_primitives.h"
#include "vulkan/texture/vulkan_draw_target.h"

using namespace igpu;

vulkan_instance_batch::vulkan_instance_batch( const config& cfg )
	: _root_batch( cfg.vk.root_batch )
{
	const auto& parameters = cfg.vk.program->instance_parameters();
	if ( parameters.count() )
	{
		if ( !cfg.vk.instance )
		{
			LOG_CRITICAL(
				"program %s expects instance parameters but none were provided",
				cfg.program->cfg().name.c_str() );
		}
		else
		{
			_job_primitives =
				vulkan_job_primitives::make( vulkan_job_primitives::config{
					cfg.vk.root_batch->vk()
						.managers->cfg()
						.queues->cfg()
						.graphics_queue,
					cfg.vk.program->pipeline_layout(),
					cfg.vk.root_batch,
					cfg.vk.root_batch->vk().swap_count,
					2,
					&parameters,
					cfg.vk.instance.get(),
				} );
		}
	}
}

void vulkan_instance_batch::enabled( bool enabled )
{
	_enabled = enabled;
}

bool vulkan_instance_batch::enabled() const
{
	return _enabled;
}

void vulkan_instance_batch::draw_params( const variant_t& draw_params )
{
	_draw_params = draw_params;
	_indirect_draw_dependency = nullptr;

	if ( std::holds_alternative< draw_indirect_parameters >( _draw_params ) )
	{
		const draw_indirect_parameters& indirect_params =
			std::get< draw_indirect_parameters >( _draw_params );

		auto buffer =
			std::dynamic_pointer_cast< vulkan_compute_buffer, compute_buffer >(
				indirect_params.buffer );

		if ( !buffer )
		{
			LOG_CRITICAL( "indirect params buffer is null" );
		}
		else if (
			0 == ( buffer->cfg().flags & compute_buff_flag::INDIRECT_DRAW ) )
		{
			LOG_CRITICAL(
				"indirect params buffer must have "
				"compute_buff_flag::INDIRECT_DRAW flag set" );
		}
		else
		{
			_indirect_draw_dependency = vulkan_job_buffers::make(
				{ _root_batch,
				  { {
					  {
						  decorator::READABLE,
						  VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
						  VK_ACCESS_INDIRECT_COMMAND_READ_BIT,
					  },
					  buffer,
				  } } } );
		}
	}
}

const vulkan_instance_batch::variant_t& vulkan_instance_batch::draw_params()
	const
{
	return _draw_params;
}
