
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/shader/vulkan_parameter.h"

namespace igpu
{
	struct vulkan_job_scope
	{
		decorator decorators = decorator::NOTHING;
		VkPipelineStageFlags stages = 0;
		VkAccessFlags access = 0;

		bool is_empty() const;
		bool is_valid() const;
		bool is_readable() const;
		bool is_writable() const;
		bool contains( const vulkan_job_scope& ) const;
		vulkan_job_scope combined( const vulkan_job_scope& ) const;
	};
}
