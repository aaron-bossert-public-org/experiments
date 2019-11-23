
#include "vulkan/buffer/vulkan_compute_buffer.h"

#include "vulkan/buffer/vulkan_staged_buffer.h"

using namespace igpu;

std::unique_ptr< vulkan_compute_buffer > vulkan_compute_buffer::make(
	const config& cfg )
{
	return vulkan_staged_buffer_t< vulkan_compute_buffer >::make( {
		cfg.vk.device,
		cfg.vk.device_properties,
		cfg.vk.vma,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		cfg.vk.managers,
		cfg.memory,
	} );
}
