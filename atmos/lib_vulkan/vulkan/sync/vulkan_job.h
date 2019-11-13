
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include <memory>
#include <vector>

namespace igpu
{
	class vulkan_fence;

	class vulkan_job
	{
	public:
		virtual const std::shared_ptr< vulkan_fence >& fence() const = 0;
	};
}