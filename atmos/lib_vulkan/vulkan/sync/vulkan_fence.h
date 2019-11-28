
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

#include <memory>

namespace igpu
{
	class vulkan_queue;

	class vulkan_fence
	{
	public:
		struct config
		{
			vulkan_queue* queue;
			VkFenceCreateInfo info = {
				VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				nullptr,
				0,
			};
		};

		static std::unique_ptr< vulkan_fence > make(
			const config&,
			ptrdiff_t submit_index );

		const config& cfg() const;

		VkFence vk_fence() const;

		ptrdiff_t submit_index() const;

		void re_submit( ptrdiff_t submit_index );

		bool is_ready() const;

		void wait() const;

		~vulkan_fence();

	private:
		vulkan_fence( const config&, VkFence, ptrdiff_t );

		const config _cfg;
		const VkDevice _device;
		const VkFence _vk_fence;
		ptrdiff_t _submit_index = 0;
	};
}
