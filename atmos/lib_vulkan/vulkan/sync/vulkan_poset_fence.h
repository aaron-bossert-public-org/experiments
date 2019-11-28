
#pragma once

#include "vulkan/defines/vulkan_includes.h"

namespace igpu
{
	class vulkan_fence;
	class vulkan_queue;

	// waiting on a VkFence requires that the fence not be in an initial or
	// reset state, and has the side effect of flushing/invalidating gpu caches.

	// poset (partially ordered) has no such requirements and cannot be relied
	// upon to sync caches. This makes it lighter weight but forces code using
	// it to rely barriers/semaphores for read/write memory synchronization.
	class vulkan_poset_fence
	{
	public:
		bool is_ready();

		void wait_or_skip();

		static vulkan_poset_fence prev_submit( vulkan_queue* );

		static vulkan_poset_fence next_submit( vulkan_queue* );

		static vulkan_poset_fence past( vulkan_queue*, ptrdiff_t submit_index );

	private:
		vulkan_queue* _queue;
		ptrdiff_t _submit_index;
	};
}
