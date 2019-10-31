
#include <vulkan/buffer/vulkan_vertex_buffer.h>
#include <vulkan/buffer/vulkan_staged_buffer.h>

using namespace igpu;

std::unique_ptr<vulkan_vertex_buffer> vulkan_vertex_buffer::make(
	const config& cfg,
	const scoped_ptr < vulkan_buffer_mediator >& buffer_mediator)
{
	vulkan_staged_buffer::config staged_cfg({
		cfg.usage,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		buffer_mediator });

	if (cfg.attributes.size() == 0)
	{
		LOG_CRITICAL("no attributes specified");
	}
	else
	{
		return vulkan_staged_buffer_t<vulkan_vertex_buffer>::make(
			cfg,
			staged_cfg);
	}

	return nullptr;
}
