
#pragma once

#include <igpu/batch/batch_nodes.h>

#include <igpu/batch/batch_utility.h>

#include <vulkan/batch/vulkan_batch_nodes.h>
#include <vulkan/buffer/vulkan_buffer.h>
#include <vulkan/buffer/vulkan_compute_buffer.h>
#include <vulkan/texture/vulkan_depth_texture2d.h>
#include <vulkan/texture/vulkan_render_texture2d.h>
#include <vulkan/texture/vulkan_texture2d.h>
#include <vulkan/texture/vulkan_image.h>

#include <vulkan/sync/vulkan_fence.h>
#include <vulkan/context/vulkan_context.h>

using namespace igpu;


void vulkan_instance_batch::draw(const vulkan_batch_stack&)
{
}

vulkan_geometry_batch::vulkan_geometry_batch(const vulkan_instance_batch::config& cfg)
	: _root_batch ( cfg.vk.root_batch )
{
}

vulkan_geometry_batch::~vulkan_geometry_batch()
{
	auto fence = _root_batch->fence();
	if (fence)
	{
		item().add_fence(fence);
	}
}

void vulkan_geometry_batch::start_draw(const vulkan_batch_stack&)
{

}

void vulkan_geometry_batch::stop_draw()
{

}

vulkan_geometry* vulkan_geometry_batch::get_key(const vulkan_instance_batch::config& cfg)
{
	return cfg.vk.geometry.get();
}
	
vulkan_material_batch::vulkan_material_batch(const vulkan_instance_batch::config& cfg)
	: _root_batch(cfg.vk.root_batch)
{

}

vulkan_material_batch::~vulkan_material_batch()
{
	auto fence = _root_batch->fence();
	if (fence)
	{
		item().visit(
			[&fence](auto&& ptr) 
			{
				ptr->gpu_resource().add_fence(fence);
			}
		);
	}
}

void vulkan_material_batch::start_draw(const vulkan_batch_stack&)
{

}

void vulkan_material_batch::stop_draw()
{

}

vulkan_primitives* vulkan_material_batch::get_key(const vulkan_instance_batch::config& cfg)
{
	return cfg.vk.material.get();
}

vulkan_render_states_batch::vulkan_render_states_batch(const vulkan_instance_batch::config& cfg)
	: _root_batch(cfg.vk.root_batch)
{

}

vulkan_render_states_batch::~vulkan_render_states_batch()
{
}

void vulkan_render_states_batch::start_draw(const vulkan_batch_stack&)
{

}

void vulkan_render_states_batch::stop_draw()
{

}

vulkan_render_states* vulkan_render_states_batch::get_key(const vulkan_instance_batch::config& cfg)
{
	return cfg.vk.render_states.get();
}

vulkan_program_batch::vulkan_program_batch(const vulkan_instance_batch::config& cfg)
	: _root_batch(cfg.vk.root_batch)
{

}

vulkan_program_batch::~vulkan_program_batch()
{
}

void vulkan_program_batch::start_draw(const vulkan_batch_stack&)
{

}

void vulkan_program_batch::stop_draw()
{

}

vulkan_program* vulkan_program_batch::get_key(const vulkan_instance_batch::config& cfg)
{
	return cfg.vk.program.get();
}

const vulkan_root_batch::config& vulkan_root_batch::cfg() const
{
	return _cfg;
}

void vulkan_root_batch::start_draw(const vulkan_batch_draw_config& draw_config)
{
	_fence = draw_config.vk.fence;
	ASSERT_CONTEXT((bool)_fence);
}

void vulkan_root_batch::stop_draw()
{

}

const std::shared_ptr<vulkan_fence>& vulkan_root_batch::fence() const
{
	return _fence;
}

std::unique_ptr<vulkan_batch_binding> vulkan_root_batch::make_binding(
	const instance_batch::config& base_cfg,
	const utility::sphere& visibility_sphere)
{
	vulkan_instance_batch::config cfg{
	base_cfg,
	{
		this,
		std::dynamic_pointer_cast < vulkan_program, program > (cfg.program),
		std::dynamic_pointer_cast < vulkan_render_states, render_states >(cfg.render_states),
		std::dynamic_pointer_cast < vulkan_primitives, primitives >(cfg.material),
		std::dynamic_pointer_cast < vulkan_geometry, geometry >(cfg.geometry),
		std::dynamic_pointer_cast < vulkan_primitives, primitives >(cfg.primitives),
	}};
	
	return batch_utility::create_binding<vulkan_batch_binding>(
		*this,
		cfg,
		visibility_sphere);
}

std::unique_ptr<vulkan_root_batch> vulkan_root_batch::make(
	const config& cfg)
{
	if (!cfg.context)
	{
		LOG_CRITICAL("context is null");
	}
	else
	{
		return std::unique_ptr<vulkan_root_batch>(
			new vulkan_root_batch(cfg));
	}

	return nullptr;
}

vulkan_root_batch::vulkan_root_batch(const config& cfg)
	: _cfg(cfg)
{
}

vulkan_root_batch::~vulkan_root_batch()
{
	if (_fence)
	{
		_fence->wait();
	}
}

vulkan_batch_binding::~vulkan_batch_binding()
{

	const auto& cfg = this->cfg();
	auto fence = cfg.vk.root_batch->fence();
	if (fence)
	{
		cfg.vk.primitives->visit(
			[&fence](auto&& ptr)
			{
				ptr->gpu_resource().add_fence(fence);
			}
		);
	}
}