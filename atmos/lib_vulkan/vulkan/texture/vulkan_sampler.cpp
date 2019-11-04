
#include "vulkan/texture/vulkan_sampler.h"

using namespace igpu;

VkSamplerAddressMode igpu::to_vulkan_address(sampler::address address)
{
	switch (address)
	{
	case sampler::address::CLAMP:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

	case sampler::address::WRAP:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;

	case sampler::address::MIRROR:
		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	};

	if (sampler::is_valid(address))
	{
		LOG_CRITICAL("unhandled format: %s", sampler::to_string(address).data());
	}
	else
	{
		LOG_CRITICAL("invalid format: %d", address);
	}

	return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}


VkFilter igpu::to_vulkan_filter(sampler::filter filter)
{
	switch (filter)
	{
	case sampler::filter::NEAREST:
		return VK_FILTER_NEAREST;

	case sampler::filter::LINEAR:
		return VK_FILTER_LINEAR;
	};

	if (sampler::is_valid(filter))
	{
		LOG_CRITICAL("unhandled format: %s", sampler::to_string(filter).data());
	}
	else
	{
		LOG_CRITICAL("invalid format: %d", filter);
	}

	return VK_FILTER_LINEAR;
}