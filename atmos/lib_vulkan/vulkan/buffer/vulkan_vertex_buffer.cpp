
#include "vulkan/buffer/vulkan_vertex_buffer.h"

#include "vulkan/buffer/vulkan_staged_buffer.h"

using namespace igpu;

std::unique_ptr< vulkan_vertex_buffer > vulkan_vertex_buffer::make(
	const config& cfg )
{
	if ( cfg.attributes.size() == 0 )
	{
		LOG_CRITICAL( "no attributes specified" );
	}
	else
	{
		return vulkan_staged_buffer_t< vulkan_vertex_buffer >::make(
			cfg,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT );
	}

	return nullptr;
}