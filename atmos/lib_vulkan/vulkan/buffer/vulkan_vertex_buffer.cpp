
#include "vulkan/buffer/vulkan_vertex_buffer.h"

#include "vulkan/buffer/vulkan_staged_buffer.h"

using namespace igpu;

std::unique_ptr< vulkan_vertex_buffer > vulkan_vertex_buffer::make(
	const config& cfg,
	const scoped_ptr< vulkan_queue_manager >& queue_manager,
	const scoped_ptr< vulkan_barrier_manager >& barrier_manager )
{
	if ( cfg.attributes.size() == 0 )
	{
		LOG_CRITICAL( "no attributes specified" );
	}
	else
	{
		return vulkan_staged_buffer_t< vulkan_vertex_buffer >::make(
			{
				cfg.vk.device,
				cfg.vk.device_properties,
				cfg.vk.vma,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				queue_manager,
				barrier_manager,
				cfg.memory,
			},
			cfg );
	}

	return nullptr;
}
const vulkan_vertex_buffer::config& vulkan_vertex_buffer::cfg() const
{
	return _cfg;
}
vulkan_vertex_buffer::vulkan_vertex_buffer( const config& cfg )
	: _cfg( cfg )
{}