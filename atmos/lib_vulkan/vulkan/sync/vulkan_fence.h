
#pragma once

#include "vulkan/defines/vulkan_includes.h"

namespace igpu
{
	class vulkan_fence
	{
	public:
		struct config
		{
			VkDevice device = nullptr;
			VkQueue queue = nullptr;
			ptrdiff_t submit_index = 0;
			VkFenceCreateInfo info = {
				VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				nullptr,
				0,
			};
		};

		static std::unique_ptr< vulkan_fence > make( const config& );

		const config& cfg() const;

		VkFence get() const;

		bool is_ready( uint64_t wait_nanosecnods = 0 );

		void wait( uint64_t err_msg_nanosecnods = 1000 * 1000 * 1000 );

		vulkan_fence( const config& );

		~vulkan_fence();

	private:
		const config _cfg;
		const VkFence _fence;
	};
}
