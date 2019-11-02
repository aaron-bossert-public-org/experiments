
#include <vulkan/texture/vulkan_draw_target.h>

#include <vulkan/texture/vulkan_render_target.h>
#include <vulkan/texture/vulkan_depth_target.h>

#include <framework/logging/log.h>

using namespace igpu;

const vulkan_draw_target::config& vulkan_draw_target::cfg() const
{
	return _cfg;
}

std::unique_ptr<vulkan_draw_target> vulkan_draw_target::make(
	const config& cfg)
{
	if (!cfg.color && !cfg.depth)
	{
		LOG_CRITICAL(
			"color and depth cannot both be null");
	}
	else if (!cfg.color != !cfg.vk.color)
	{
		LOG_CRITICAL(
			"failed to cast color target to vulkan color target");
	}
	else if (!cfg.depth != !cfg.vk.depth)
	{
		LOG_CRITICAL(
			"failed to cast depth target to vulkan depth target");
	}
	else
	{
		return std::unique_ptr<vulkan_draw_target>(
			new vulkan_draw_target(cfg));
	}
	
	return nullptr;
}


vulkan_draw_target::vulkan_draw_target(
	const config& cfg)
	: _cfg(cfg)
{
}
