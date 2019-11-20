
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_queue;

	class vulkan_command_buffer
	{
	public:
		struct config
		{
			scoped_ptr< vulkan_queue > queue;
			VkCommandPool command_pool = nullptr;
			VkCommandBufferLevel level;
		};

		VkCommandBuffer vk_cmds() const;

		~vulkan_command_buffer();

		vulkan_command_buffer( const config& );

	private:
		const config _cfg;
		VkCommandBuffer _command_buffer = nullptr;
	};
}
