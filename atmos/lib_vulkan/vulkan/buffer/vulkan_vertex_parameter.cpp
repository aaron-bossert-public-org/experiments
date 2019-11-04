
#include "vulkan/buffer/vulkan_vertex_parameter.h"

using namespace igpu;

VkFormat igpu::to_vulkan_format( components format )
{
	switch ( format )
	{
	case components::FLOAT1:
		return VK_FORMAT_R32_SFLOAT;
	case components::FLOAT2:
		return VK_FORMAT_R32G32_SFLOAT;
	case components::FLOAT3:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case components::FLOAT4:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	};

	if ( is_valid( format ) )
	{
		LOG_CRITICAL( "unhandled format: %s", to_string( format ).data() );
	}
	else
	{
		LOG_CRITICAL( "invalid format: %d", format );
	}

	return VK_FORMAT_UNDEFINED;
}

const vulkan_vertex_parameter::config& vulkan_vertex_parameter::cfg() const
{
	return _cfg;
}

vulkan_vertex_parameter::vulkan_vertex_parameter(
	const spirv::vertex_parameter& cfg )
	: _cfg{
		  cfg,
		  to_vulkan_format( cfg.components ),
	  }
{}
