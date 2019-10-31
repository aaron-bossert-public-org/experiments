
#pragma once

#include <igpu/material/shader_stages.h>

#include <vulkan/defines/vulkan_includes.h>

namespace igpu
{
	VkShaderStageFlagBits to_vulkan_stage_flags(shader_stages);

	shader_stages from_vulkan_stage_flags(VkShaderStageFlagBits);
}
