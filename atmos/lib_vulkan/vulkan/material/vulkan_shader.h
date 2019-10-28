
#pragma once

#include <igpu/material/program_parsing.h>

namespace igpu
{
	class vulkan_shader
	{
	public:

		virtual VkPipelineShaderStageCreateInfo shader_stage_info() = 0;

		virtual size_t resource_count() const = 0;

		virtual const spirv_resource& resource(size_t) const = 0;
	};
}
