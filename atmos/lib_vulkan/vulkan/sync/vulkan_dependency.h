
#pragma once

#include <memory>
#include <vector>

namespace igpu
{
	class vulkan_fence;
	
	class vulkan_resource
	{
	public:

		void add_fence(const std::shared_ptr<vulkan_fence>&);

		void wait_on_fences();

		~vulkan_resource();

	private:

		std::vector<std::shared_ptr<vulkan_fence>> _fences;
	};
}
