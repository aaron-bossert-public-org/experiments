
#pragma once

#include <igpu/batch/batch_nodes.h>

#include <igpu/batch/batch_utility.h>

#include <vulkan/batch/vulkan_batch_nodes.h>

using namespace igpu;

vulkan_geometry_batch::vulkan_geometry_batch(const vulkan_instance_batch::config&)
{

}

vulkan_geometry* vulkan_geometry_batch::get_key(const vulkan_instance_batch::config& cfg)
{
	return cfg.vulkan.geometry.get();
}
	
vulkan_material_batch::vulkan_material_batch(const vulkan_instance_batch::config&)
{

}

material* vulkan_material_batch::get_key(const vulkan_instance_batch::config& cfg)
{
	return cfg.material.get();
}

vulkan_render_state_batch::vulkan_render_state_batch(const vulkan_instance_batch::config&)
{

}

vulkan_render_states* vulkan_render_state_batch::get_key(const vulkan_instance_batch::config& cfg)
{
	return cfg.vulkan.render_states.get();
}

vulkan_program_batch::vulkan_program_batch(const vulkan_instance_batch::config&)
{

}

vulkan_program* vulkan_program_batch::get_key(const vulkan_instance_batch::config& cfg)
{
	return cfg.vulkan.program.get();
}

vulkan_root_batch::vulkan_root_batch(const config& cfg)
: _cfg(cfg)
{
}

const vulkan_root_batch::config& vulkan_root_batch::cfg() const
{
	return _cfg;
}

std::unique_ptr<batch_binding> vulkan_root_batch::make_binding(
	const vulkan_instance_batch::config& cfg,
	const utility::sphere& visibility_sphere)
{
	return batch_utility::create_binding(*this, cfg, visibility_sphere);
}