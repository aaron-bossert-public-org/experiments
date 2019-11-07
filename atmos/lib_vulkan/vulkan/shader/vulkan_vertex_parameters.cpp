
#include "vulkan/shader/vulkan_vertex_parameters.h"

using namespace igpu;

vulkan_vertex_parameters::vulkan_vertex_parameters(
	std::vector< vulkan_vertex_parameter >&& parameters )
	: _parameters( std::move( parameters ) )
{}

size_t vulkan_vertex_parameters::count() const
{
	return _parameters.size();
}

const vulkan_vertex_parameter& vulkan_vertex_parameters::parameter(
	size_t i ) const
{
	return _parameters[i];
}
