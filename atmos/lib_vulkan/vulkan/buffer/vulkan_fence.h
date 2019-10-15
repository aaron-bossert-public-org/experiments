
#pragma once

// Vulkan implementation includes - begin
#include <vulkan/defines/vulkan_includes.h>
// Vulkan implementation includes - end

namespace igpu
{
	class vulkan_fence
	{
	public:

		struct config
		{
			VkDevice device = nullptr;
			VkFenceCreateInfo info = {
				VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				nullptr,
				0,
			};
		};

		static std::unique_ptr<vulkan_fence> make(const config&);

		VkFence get() const;

		bool is_ready(uint64_t wait_nanosecnods = 0);

		void wait(uint64_t err_msg_nanosecnods = 1000 * 1000 * 1000);

		vulkan_fence(const config&);

		~vulkan_fence();
	
	private:

		const config _cfg;
		const VkFence _fence;
	};
}
