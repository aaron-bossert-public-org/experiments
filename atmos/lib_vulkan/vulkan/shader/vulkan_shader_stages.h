
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/shader/shader_stages.h"

namespace igpu
{
	VkShaderStageFlagBits to_vulkan_shader_stage_flags( shader_stages );

	shader_stages from_vulkan_shader_stage_flags( VkShaderStageFlagBits );

	VkPipelineStageFlags to_vulkan_pipeline_stage_flags( shader_stages );
}
