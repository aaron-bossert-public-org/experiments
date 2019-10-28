
#include <vulkan/material/vulkan_parameter.h>

using namespace igpu;

const vulkan_parameter::config& vulkan_parameter::cfg() const
{
	return _cfg;
}

vulkan_parameter::vulkan_parameter(const config& cfg)
	: _cfg(cfg)
{
}
