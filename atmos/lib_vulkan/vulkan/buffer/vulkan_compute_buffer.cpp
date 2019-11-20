
#include "vulkan/buffer/vulkan_compute_buffer.h"

#include "vulkan/buffer/vulkan_staged_buffer.h"

using namespace igpu;

std::unique_ptr< vulkan_compute_buffer > vulkan_compute_buffer::make(
	const config& cfg,
	const scoped_ptr< vulkan_queues >& queues,
	const scoped_ptr< vulkan_barrier_manager >& barrier_manager )
{
	return vulkan_staged_buffer_t< vulkan_compute_buffer >::make( {
		cfg.vk.device,
		cfg.vk.device_properties,
		cfg.vk.vma,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		queues,
		barrier_manager,
		cfg.memory,
	} );
}
