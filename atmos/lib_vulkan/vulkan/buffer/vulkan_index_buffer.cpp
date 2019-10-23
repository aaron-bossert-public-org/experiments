
#include <vulkan/buffer/vulkan_index_buffer.h>
#include <vulkan/buffer/vulkan_staged_buffer.h>

using namespace igpu;

VkIndexType igpu::to_vulkan_format(index_format format)
{
	switch (format)
	{
	case index_format::UNSIGNED_INT:
		return VK_INDEX_TYPE_UINT32;
	case index_format::UNSIGNED_SHORT:
		return VK_INDEX_TYPE_UINT16;
	};

	if (is_valid(format))
	{
		LOG_CRITICAL("unhandled format: %s", to_string(format).data());
	}
	else
	{
		LOG_CRITICAL("invalid format: %d", format);
	}

	return VK_INDEX_TYPE_UINT16;
}

std::unique_ptr<vulkan_index_buffer> vulkan_index_buffer::make(
	const config& cfg,
	const scoped_ptr < vulkan_buffer_mediator >& buffer_mediator)
{
	vulkan_staged_buffer::config buffer_cfg({
		cfg.usage,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		buffer_mediator });

	if (vulkan_staged_buffer::validate(buffer_cfg))
	{
		return std::unique_ptr<vulkan_index_buffer>(
			new vulkan_staged_buffer_t < vulkan_index_buffer > (buffer_cfg, cfg));
	}

	return nullptr;
}

const vulkan_index_buffer::config& vulkan_index_buffer::cfg() const
{
	return _cfg;
}

VkIndexType vulkan_index_buffer::format()
{
	return to_vulkan_format(_cfg.format);
}

vulkan_index_buffer::vulkan_index_buffer(
	const config& cfg)
	: _cfg(cfg)
{
}
