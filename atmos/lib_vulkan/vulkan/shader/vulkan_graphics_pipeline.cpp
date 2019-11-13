
#include "vulkan/shader/vulkan_graphics_pipeline.h"

using namespace igpu;

const vulkan_graphics_pipeline::config& vulkan_graphics_pipeline::cfg() const
{
	return _cfg;
}

std::unique_ptr< vulkan_graphics_pipeline > vulkan_graphics_pipeline::make(
	const config& cfg )
{
	return std::unique_ptr< vulkan_graphics_pipeline >(
		new vulkan_graphics_pipeline( cfg ) );
}

vulkan_graphics_pipeline::~vulkan_graphics_pipeline()
{}

vulkan_graphics_pipeline::vulkan_graphics_pipeline( const config& cfg )
	: _cfg( cfg )
{}