
#include "vulkan/shader/vulkan_parameters.h"

using namespace igpu;

vulkan_parameters::vulkan_parameters( config&& cfg )
	: _cfg( std::move( cfg ) )
{}

const vulkan_parameters::config& vulkan_parameters::cfg() const
{
	return _cfg;
}

vulkan_parameters::~vulkan_parameters()
{
	vkDestroyDescriptorSetLayout( _cfg.vk.device, _cfg.vk.layout, nullptr );
}

size_t vulkan_parameters::count() const
{
	return _cfg.vk.parameters.size();
}

const vulkan_parameter& vulkan_parameters::parameter( size_t i ) const
{
	return _cfg.vk.parameters[i];
}
