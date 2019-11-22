
#pragma once


#include "vulkan/batch/vulkan_instance_batch.h"

#include "vulkan/batch/vulkan_batch_nodes.h"
#include "vulkan/manager/vulkan_queue_manager.h"
#include "vulkan/sync/vulkan_job_primitives.h"
#include "vulkan/texture/vulkan_draw_target.h"

using namespace igpu;

vulkan_instance_batch::vulkan_instance_batch( const config& cfg )
{
	if ( cfg.vk.instance )
	{
		_job_primitives =
			vulkan_job_primitives::make( vulkan_job_primitives::config{
				cfg.vk.root_batch->vk()
					.draw_target->cfg()
					.vk.queue_manager->cfg()
					.graphics_queue,
				cfg.vk.program->pipeline_layout(),
				cfg.vk.root_batch,
				cfg.vk.root_batch->vk().swap_count,
				2,
				&cfg.vk.program->instance_parameters(),
				cfg.vk.instance.get(),
			} );
	}
}

void vulkan_instance_batch::enabled( bool enabled )
{
	_enabled = enabled;
}

void vulkan_instance_batch::base_vertex(
	const std::optional< ptrdiff_t >& base_vertex )
{
	_base_vertex = base_vertex;
}

void vulkan_instance_batch::instance_start(
	const std::optional< size_t >& instance_start )
{
	_instance_start = instance_start;
}

void vulkan_instance_batch::instance_count(
	const std::optional< size_t >& instance_count )
{
	_instance_count = instance_count;
}

void vulkan_instance_batch::element_start(
	const std::optional< size_t >& element_start )
{
	_element_start = element_start;
}

void vulkan_instance_batch::element_count(
	const std::optional< size_t >& element_count )
{
	_element_count = element_count;
}

void vulkan_instance_batch::visibility_sphere(
	const std::optional< utility::sphere >& visibility_sphere )
{
	_visibility_sphere = visibility_sphere;
}

bool vulkan_instance_batch::enabled() const
{
	return _enabled;
}

const std::optional< ptrdiff_t >& vulkan_instance_batch::base_vertex() const
{
	return _base_vertex;
}

const std::optional< size_t >& vulkan_instance_batch::instance_start() const
{
	return _instance_start;
}

const std::optional< size_t >& vulkan_instance_batch::instance_count() const
{
	return _instance_count;
}

const std::optional< size_t >& vulkan_instance_batch::element_start() const
{
	return _element_start;
}

const std::optional< size_t >& vulkan_instance_batch::element_count() const
{
	return _element_count;
}

const std::optional< utility::sphere >& vulkan_instance_batch::
	visibility_sphere() const
{
	return _visibility_sphere;
}