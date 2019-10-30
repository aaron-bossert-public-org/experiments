
#pragma once

#include <igpu/batch/batch_nodes.h>

#include <igpu/batch/batch_utility.h>

#include <vulkan/batch/vulkan_batch_nodes.h>
#include <vulkan/context/vulkan_context.h>

using namespace igpu;


void vulkan_instance_batch::draw(const vulkan_batch_stack&)
{

}

vulkan_geometry_batch::vulkan_geometry_batch(const vulkan_instance_batch::config&)
{

}

void vulkan_geometry_batch::start_draw(const vulkan_batch_stack&)
{

}

void vulkan_geometry_batch::stop_draw()
{

}

vulkan_geometry* vulkan_geometry_batch::get_key(const vulkan_instance_batch::config& cfg)
{
	return cfg.vulkan.geometry.get();
}
	
vulkan_material_batch::vulkan_material_batch(const vulkan_instance_batch::config&)
{

}

void vulkan_material_batch::start_draw(const vulkan_batch_stack&)
{

}

void vulkan_material_batch::stop_draw()
{

}

material* vulkan_material_batch::get_key(const vulkan_instance_batch::config& cfg)
{
	return cfg.material.get();
}

vulkan_render_state_batch::vulkan_render_state_batch(const vulkan_instance_batch::config&)
{

}

void vulkan_render_state_batch::start_draw(const vulkan_batch_stack&)
{

}

void vulkan_render_state_batch::stop_draw()
{

}

vulkan_render_states* vulkan_render_state_batch::get_key(const vulkan_instance_batch::config& cfg)
{
	return cfg.vulkan.render_states.get();
}

vulkan_program_batch::vulkan_program_batch(const vulkan_instance_batch::config&)
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
	return cfg.vulkan.program.get();
}

const vulkan_root_batch::config& vulkan_root_batch::cfg() const
{
	return _cfg;
}

void vulkan_root_batch::start_draw(const vulkan_batch_stack&)
{

}

void vulkan_root_batch::stop_draw()
{

}

std::shared_ptr<batch_binding> vulkan_root_batch::make_binding(
	const instance_batch::config& base_cfg,
	const utility::sphere& visibility_sphere)
{
	vulkan_instance_batch::config cfg;
	COPY_TO_DERRIVED_CONFIG(base_cfg, &cfg);
	cfg.vulkan.program = std::dynamic_pointer_cast < vulkan_program, program > (cfg.program);
	cfg.vulkan.render_states = std::dynamic_pointer_cast < vulkan_render_states, render_states >(cfg.render_states);
	cfg.vulkan.geometry = std::dynamic_pointer_cast <vulkan_geometry, geometry >(cfg.geometry);
	
	return batch_utility::create_binding(
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
