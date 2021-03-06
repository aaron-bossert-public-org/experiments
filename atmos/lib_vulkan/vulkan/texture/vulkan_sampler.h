
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/texture/sampler.h"
#include "igpu/texture/texture2d.h"

namespace igpu
{
	VkSamplerAddressMode to_vulkan_address( sampler::address );

	VkFilter to_vulkan_filter( sampler::filter );
}