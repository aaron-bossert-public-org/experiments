
#include "vulkan/buffer/vulkan_compute_buffer.h"

#include "vulkan/buffer/vulkan_staged_buffer.h"

using namespace igpu;

std::unique_ptr< vulkan_compute_buffer > vulkan_compute_buffer::make(
	const config& cfg,
	const scoped_ptr< vulkan_synchronization >& synchronization )
{
	return vulkan_staged_buffer_t< vulkan_compute_buffer >::make( {
		cfg.vk.device_properties,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		synchronization,
		cfg.memory,
	} );
}
