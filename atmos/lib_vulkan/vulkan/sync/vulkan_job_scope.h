
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/shader/vulkan_parameter.h"

namespace igpu
{
	struct vulkan_job_scope
	{
		decorator decorators = decorator::NOTHING;
		VkPipelineStageFlags stages = VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
		VkAccessFlags access = VK_ACCESS_FLAG_BITS_MAX_ENUM;

		bool is_readable() const;
		bool is_writable() const;
		bool contains( const vulkan_job_scope& ) const;
		vulkan_job_scope combined( const vulkan_job_scope& ) const;
	};
}
