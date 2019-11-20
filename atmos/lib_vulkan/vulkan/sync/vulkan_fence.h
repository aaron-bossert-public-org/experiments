
#pragma once

#include "vulkan/defines/vulkan_includes.h"

namespace igpu
{
	class vulkan_queue;

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

		static std::unique_ptr< vulkan_fence > make( const config& );

		const config& cfg() const;

		VkFence vk_fence() const;

		bool is_ready( ptrdiff_t submit_index, uint64_t wait_nanosecnods = 0 );

		void wait(
			ptrdiff_t submit_index,
			uint64_t err_msg_nanosecnods = 1000 * 1000 * 1000 );

		void on_submit( const vulkan_queue& );

		ptrdiff_t submit_index() const;

		vulkan_fence( const config& );

		~vulkan_fence();

	private:
		const config _cfg;
		const VkFence _fence;
		ptrdiff_t _submit_index = 0;
	};
}
