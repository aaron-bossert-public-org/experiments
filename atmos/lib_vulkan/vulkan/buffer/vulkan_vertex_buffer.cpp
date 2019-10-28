
#include <vulkan/buffer/vulkan_vertex_buffer.h>
#include <vulkan/buffer/vulkan_staged_buffer.h>

using namespace igpu;


VkFormat igpu::to_vulkan_format(components format)
{
	switch (format)
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

	if (is_valid(format))
	{
		LOG_CRITICAL("unhandled format: %s", to_string(format).data());
	}
	else
	{
		LOG_CRITICAL("invalid format: %d", format);
	}

	return VK_FORMAT_UNDEFINED;
}

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
