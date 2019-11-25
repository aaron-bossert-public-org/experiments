
#include "vulkan/shader/vulkan_constant_parameters.h"

#include "igpu/shader/constant_parameter.h"

using namespace igpu;

vulkan_constant_parameters::vulkan_constant_parameters( config&& cfg )
	: _cfg( std::move( cfg ) )
{}

const vulkan_constant_parameters::config& vulkan_constant_parameters::cfg()
	const
{
	return _cfg;
}

vulkan_constant_parameters::~vulkan_constant_parameters()
{}
