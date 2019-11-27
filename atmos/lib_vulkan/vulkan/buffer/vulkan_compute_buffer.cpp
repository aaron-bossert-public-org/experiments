
#include "vulkan/buffer/vulkan_compute_buffer.h"

#include "vulkan/buffer/vulkan_staged_buffer.h"

using namespace igpu;

std::unique_ptr< vulkan_compute_buffer > vulkan_compute_buffer::make(
	const config& cfg )
{
	VkBufferUsageFlags vk_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	if ( 0 != ( cfg.flags & compute_buff_flag::INDIRECT_DRAW ) )
	{
		vk_usage_flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
	}

	return vulkan_staged_buffer_t< vulkan_compute_buffer >::make(
		cfg,
		(VkBufferUsageFlagBits)vk_usage_flags );
}
