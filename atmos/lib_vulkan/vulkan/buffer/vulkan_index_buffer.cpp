
#include <vulkan/buffer/vulkan_index_buffer.h>

using namespace igpu;

std::unique_ptr<vulkan_index_buffer> vulkan_index_buffer::make(
	const config& cfg,
	const std::shared_ptr < vulkan_buffer_mediator >& buffer_mediator)
{
	vulkan_staged_buffer::config buffer_cfg({
		cfg.usage,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		buffer_mediator });

	if (vulkan_staged_buffer::validate(buffer_cfg))
	{
		return std::unique_ptr<vulkan_index_buffer>(new vulkan_index_buffer(cfg, buffer_cfg));
	}

	return nullptr;
}

vulkan_index_buffer::vulkan_index_buffer(
	const config& cfg,
	const vulkan_staged_buffer::config& buffer_cfg)
	: vulkan_staged_buffer_t (
		cfg,
		buffer_cfg)
{
}
