
#pragma once

#include "vulkan/defines/vulkan_includes.h"

namespace igpu
{
	class vulkan_queue;

	// waiting on a VkFence requires that the fence has not been reset to
	// its initial state, and has the side effect of flushing/invalidating
	// gpu caches.

	// partially ordered fence has no such requirements and may not sync caches.
	// This makes it lighter weight but forces code using it to rely
	// barriers/semaphores for read/write memory synchronization.
	class vulkan_poset_fence
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

		static std::unique_ptr< vulkan_poset_fence > make( const config& );

		const config& cfg() const;

		VkFence vk_fence() const;

		bool is_ready( ptrdiff_t submit_index );


		void wait_or_skip( ptrdiff_t submit_index );

		void on_submit( const vulkan_queue& );

		ptrdiff_t submit_index() const;

		vulkan_poset_fence( const config& );

		~vulkan_poset_fence();

	private:
		const config _cfg;
		const VkFence _fence;
		ptrdiff_t _submit_index = 0;
	};
}
