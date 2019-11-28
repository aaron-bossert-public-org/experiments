
#include "vulkan/compute/vulkan_compute_binding.h"

#include "vulkan/compute/vulkan_compute_pipeline.h"
#include "vulkan/compute/vulkan_compute_program.h"
#include "vulkan/manager/vulkan_managers.h"
#include "vulkan/manager/vulkan_queue_manager.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_command_pool.h"
#include "vulkan/sync/vulkan_job_primitives.h"
#include "vulkan/sync/vulkan_queue.h"

using namespace igpu;


const vulkan_compute_binding::config& vulkan_compute_binding::cfg() const
{
	return _cfg;
}
void vulkan_compute_binding::dispatch()
{
	const auto& queue = _cfg.vk.managers->cfg().queues->cfg().compute_queue;

	vulkan_job::submit_activated_dependency_barriers(
		queue,
		_cfg.vk.managers.get() );


	auto& swap_state = current_swap_state();
	auto vk_cmds = swap_state.cmds->vk_cmds();
	swap_state.fence.wait_or_skip();
	VkCommandBufferBeginInfo begin_info = {};

	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer( vk_cmds, &begin_info );

	vkCmdBindPipeline(
		vk_cmds,
		VK_PIPELINE_BIND_POINT_COMPUTE,
		_cfg.vk.pipeline->vk_pipeline() );

	if ( _st.batch_job_primitives )
	{
		_st.batch_job_primitives->record_cmds( swap_state.cmds );
	}
	if ( _st.material_job_primitives )
	{
		_st.material_job_primitives->record_cmds( swap_state.cmds );
	}
	if ( _st.instance_job_primitives )
	{
		_st.instance_job_primitives->record_cmds( swap_state.cmds );
	}

	vkCmdDispatch(
		vk_cmds,
		_st.compute_params.group_count_x,
		_st.compute_params.group_count_y,
		_st.compute_params.group_count_z );

	vkEndCommandBuffer( vk_cmds );
	queue->submit_command( *swap_state.cmds );

	swap_state.fence = vulkan_poset_fence::prev_submit( queue.get() );
	vulkan_job::fence( swap_state.fence );
	_st.swap_index = ( _st.swap_index + 1 ) % _cfg.vk.swap_count;
}

void vulkan_compute_binding::compute_params(
	const compute_parameters& compute_params )
{
	_st.compute_params = compute_params;
}

const compute_parameters& vulkan_compute_binding::compute_params() const
{
	return _st.compute_params;
}

vulkan_job::state& vulkan_compute_binding::job_state()
{
	return _job_state;
}

const vulkan_job::state& vulkan_compute_binding::job_state() const
{
	return _job_state;
}

vulkan_compute_binding::~vulkan_compute_binding()
{}

std::unique_ptr< vulkan_compute_binding > vulkan_compute_binding::make(
	const config& cfg )
{
	if ( !cfg.vk.managers )
	{
		LOG_CRITICAL( "managers is null" );
	}
	else if ( !cfg.vk.program )
	{
		LOG_CRITICAL( "program is null" );
	}
	else if ( !cfg.vk.pipeline )
	{
		LOG_CRITICAL( "pipeline is null" );
	}
	else
	{
		const auto& program = cfg.vk.program;
		const auto& queue = cfg.vk.managers->cfg().queues->cfg().compute_queue;
		const auto& batch_parameters = program->batch_parameters();
		auto unique = std::unique_ptr< vulkan_compute_binding >(
			new vulkan_compute_binding( cfg ) );

		bool success = true;

		if ( batch_parameters.count() )
		{
			if ( !cfg.batch )
			{
				LOG_CRITICAL(
					"program %s expects batch parameters but none were "
					"provided",
					program->cfg().name.c_str() );
				success = false;
			}
			else
			{
				unique->_st.batch_job_primitives =
					vulkan_job_primitives::make( vulkan_job_primitives::config{
						queue,
						VK_PIPELINE_BIND_POINT_COMPUTE,
						program->pipeline_layout(),
						unique.get(),
						cfg.vk.swap_count,
						0,
						&batch_parameters,
						cfg.vk.batch.get(),
					} );
			}
		}
		const auto& material_parameters = program->material_parameters();
		if ( material_parameters.count() )
		{
			if ( !cfg.material )
			{
				LOG_CRITICAL(
					"program %s expects material parameters but none were "
					"provided",
					program->cfg().name.c_str() );
				success = false;
			}
			else
			{
				unique->_st.material_job_primitives =
					vulkan_job_primitives::make( vulkan_job_primitives::config{
						queue,
						VK_PIPELINE_BIND_POINT_COMPUTE,
						program->pipeline_layout(),
						unique.get(),
						cfg.vk.swap_count,
						1,
						&material_parameters,
						cfg.vk.material.get(),
					} );
			}
		}
		const auto& instance_parameters = program->instance_parameters();
		if ( instance_parameters.count() )
		{
			if ( !cfg.instance )
			{
				LOG_CRITICAL(
					"program %s expects instance parameters but none were "
					"provided",
					program->cfg().name.c_str() );
				success = false;
			}
			else
			{
				unique->_st.instance_job_primitives =
					vulkan_job_primitives::make( vulkan_job_primitives::config{
						queue,
						VK_PIPELINE_BIND_POINT_COMPUTE,
						program->pipeline_layout(),
						unique.get(),
						cfg.vk.swap_count,
						2,
						&instance_parameters,
						cfg.vk.instance.get(),
					} );
			}
		}

		unique->_st.command_pool = vulkan_command_pool::make( {
			queue,
			VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		} );

		if ( !unique->_st.command_pool )
		{
			success = false;
		}
		else
		{
			unique->_st.swap_states.resize( cfg.vk.swap_count );

			for ( size_t i = 0; i < cfg.vk.swap_count; ++i )
			{
				auto& ss = unique->_st.swap_states[i];
				ss.cmds.reset( new vulkan_command_buffer( {
					unique->_st.command_pool,
					VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				} ) );
			}
		}

		if ( success )
		{
			return unique;
		}
	}

	return nullptr;
}

vulkan_compute_binding::vulkan_compute_binding( const config& cfg )
	: _cfg( cfg )
{}

vulkan_compute_binding::swap_state& vulkan_compute_binding::current_swap_state()
{
	return _st.swap_states[_st.swap_index];
}