
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_command_buffer;
	class vulkan_queue;

	class vulkan_command_pool
	{
	public:
		struct config
		{
			scoped_ptr< vulkan_queue > queue;
			VkCommandPoolCreateFlags flags;
		};

		const config& cfg() const;

		void get_vk_pool( vulkan_command_buffer* );

		static std::unique_ptr< vulkan_command_pool > make( const config& );

		~vulkan_command_pool();

	private:
		friend class vulkan_queue;

		vulkan_command_pool( const config& );

	private:
		config _cfg;
		VkDevice _device = nullptr;
		VkCommandPool _vk_pool = nullptr;
	};
}
