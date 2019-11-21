
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_queue;
	class vulkan_command_pool;

	class vulkan_command_buffer
	{
	public:
		struct config
		{
			scoped_ptr< vulkan_command_pool > command_pool;
			VkCommandBufferLevel level;
		};

		VkCommandBuffer vk_cmds() const;

		~vulkan_command_buffer();

		vulkan_command_buffer( const config& );

	private:
		friend class vulkan_command_pool;

		void vk_pool( VkCommandPool );

	private:
		const config _cfg;
		VkCommandPool _vk_pool = nullptr;
		VkCommandBuffer _vk_cmds = nullptr;
	};
}
