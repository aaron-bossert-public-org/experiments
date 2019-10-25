
#include <vulkan/buffer/vulkan_compute_buffer.h>
#include <vulkan/buffer/vulkan_staged_buffer.h>

using namespace igpu;

std::unique_ptr<vulkan_compute_buffer> vulkan_compute_buffer::make(
	const config& cfg,
	const scoped_ptr < vulkan_buffer_mediator >& buffer_mediator)
{
	vulkan_staged_buffer::config buffer_cfg({
		cfg.usage,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		buffer_mediator });

	return vulkan_staged_buffer_t < vulkan_compute_buffer >::make (
		cfg, 
		buffer_cfg);

	return nullptr;
}
