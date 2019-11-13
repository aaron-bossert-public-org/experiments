
#pragma once

#include "framework/utility/scoped_ptr.h"

#include <vector>

namespace igpu
{
	class vulkan_fence;

	class vulkan_gpu_object
	{
	public:
		class state
		{
		public:
			friend class vulkan_gpu_object;
		};
	};
}
