
#include "vulkan/shader/vulkan_graphics_pipeline.h"

using namespace igpu;
/*
VkPipelineInputAssemblyStateCreateInfo

	cfg.vk.input_assembly_info.sType =
VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	cfg.vk.input_assembly_info.topology = to_vulkan_topology(cfg.topology);
	cfg.vk.input_assembly_info.primitiveRestartEnable = VK_FALSE;




*/


const vulkan_graphics_pipeline::config& vulkan_graphics_pipeline::cfg() const
{
	return _cfg;
}

vulkan_resource::state& vulkan_graphics_pipeline::resource_state()
{
	return _resource_state;
}

std::unique_ptr< vulkan_graphics_pipeline > vulkan_graphics_pipeline::make(
	const config& cfg )
{
	return std::unique_ptr< vulkan_graphics_pipeline >(
		new vulkan_graphics_pipeline( cfg ) );
}

vulkan_graphics_pipeline::~vulkan_graphics_pipeline()
{
	vulkan_resource::wait_on_fences();
}

vulkan_graphics_pipeline::vulkan_graphics_pipeline( const config& cfg )
	: _cfg( cfg )
{}