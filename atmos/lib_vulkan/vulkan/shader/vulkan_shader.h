
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/shader/program_parsing.h"

namespace igpu
{
	class vulkan_shader
	{
	public:
		struct vulkan
		{
			VkDevice device = nullptr;
			VkShaderStageFlagBits stage_flags = {};
		};

		virtual const vulkan& vk() const = 0;

		virtual VkPipelineShaderStageCreateInfo stage_info() const = 0;

		virtual size_t parameter_count() const = 0;

		virtual const parameter::config& parameter( size_t ) const = 0;
	};
}
