
#include <vulkan/context/vulkan_back_buffer.h>

#include <framework/logging/log.h>

// Vulkan implementation includes - begin
#include <vulkan/texture/vulkan_color_buffer.h>
#include <vulkan/texture/vulkan_depth_buffer.h>
// Vulkan implementation includes - end

using namespace igpu;

const vulkan_back_buffer::config& vulkan_back_buffer::cfg() const
{
	return _cfg;
}

std::unique_ptr<vulkan_back_buffer> vulkan_back_buffer::make(const config& cfg)
{
	if (!cfg.color)
	{
		LOG_CONTEXT(CRITICAL, "cfg.color is null");
	}
	else if (!cfg.depth)
	{
		LOG_CONTEXT(CRITICAL, "cfg.depth is null");
	}
	else
	{
		return std::unique_ptr<vulkan_back_buffer>(new vulkan_back_buffer(cfg));
	}

	return nullptr;
}

vulkan_back_buffer::vulkan_back_buffer(const config& cfg)
	: _cfg(cfg)
{
}