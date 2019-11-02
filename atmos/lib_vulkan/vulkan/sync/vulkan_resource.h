
#pragma once

#include <memory>
#include <vector>

namespace igpu
{
	class vulkan_fence;
	
	class vulkan_resource
	{
	public:

		class state
		{
		public:
			friend class vulkan_resource;

			~state();

		private:

			std::vector<std::shared_ptr<vulkan_fence>> fences;
		};

		virtual state& resource_state() = 0;

		void add_fence(const std::shared_ptr<vulkan_fence>&);

		void wait_on_fences();
	};
}
