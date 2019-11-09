
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_queue;
	class vulkan_fence;

	class vulkan_command_buffer
	{
	public:
		struct config
		{
			VkDevice device = nullptr;
			VkCommandPool command_pool = nullptr;
			VkCommandBufferLevel level;
		};

		VkCommandBuffer get();

		scoped_ptr< vulkan_fence > fence() const;

		~vulkan_command_buffer();

		vulkan_command_buffer( const config& );

		vulkan_command_buffer(
			const config&,
			const std::shared_ptr< vulkan_fence >& );

	private:
		const config _cfg;
		VkCommandBuffer _command_buffer = nullptr;
		std::shared_ptr< vulkan_fence > _fence;
	};
}
