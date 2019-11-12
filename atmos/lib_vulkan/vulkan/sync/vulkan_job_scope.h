
#pragma once

#include "vulkan/sync/vulkan_queue.h"

#include "framework/utility/scoped_ptr.h"

#include <optional>

namespace igpu
{
	struct vulkan_job_scope
	{
		VkPipelineStageFlags stages = VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
		VkAccessFlags access = VK_ACCESS_FLAG_BITS_MAX_ENUM;

		bool is_read_only() const;
		bool contains( const vulkan_job_scope& ) const;
		vulkan_job_scope combined( const vulkan_job_scope& ) const;
	};
}
