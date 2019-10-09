
#include <vulkan/material/vulkan_parameter.h>

#include <framework/logging/log.h>
#include <unordered_map>

using namespace igpu;

vulkan_parameter::vulkan_parameter()
	: parameter({}, (size_t)-1)
{
}

vulkan_parameter::vulkan_parameter(
	const config& cfg,
	size_t constraint_index)
	: parameter(cfg, constraint_index)
{
}
