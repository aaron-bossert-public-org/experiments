
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

SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
	}

	uint32_t present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}