
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

	if (!is_valid(cfg.format))
	{
		LOG_CRITICAL("invalid format: %d", cfg.format);
	}
	else
	{
		VkIndexType vulkan_format = to_vulkan_format(cfg.format);

		return vulkan_staged_buffer_t < vulkan_index_buffer > ::make(
			cfg, 
			buffer_cfg, 
			vulkan_format);
	}
	return nullptr;
}

VkIndexType vulkan_index_buffer::format() const
{
	return _format;
}

vulkan_index_buffer::vulkan_index_buffer(VkIndexType format)
	: _format(format)
{
}
