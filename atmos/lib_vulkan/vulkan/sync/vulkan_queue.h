
#pragma once

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/defines/vulkan_includes.h"

#include <list>
namespace igpu
{
	class vulkan_command_buffer;
	class vulkan_buffer;
	class vulkan_fence;

	class vulkan_queue
	{
	public:
		struct config
		{
			VkDevice device;
			uint32_t family_index = ~0U;
			uint32_t index = ~0U;
		};

		static std::unique_ptr< vulkan_queue > make( const config& );

		const config& cfg() const;

		VkQueue get();

		VkCommandPool command_pool();

		std::list< vulkan_command_buffer >& pending_commands();

		void free_completed_commands();

		ptrdiff_t get_increment_submit_index();

		~vulkan_queue();

	private:
		vulkan_queue(
			const config&,
			VkQueue queue,
			VkCommandPool command_pool );

	private:
		VkQueue _queue = nullptr;
		VkCommandPool _command_pool = nullptr;
		std::list< vulkan_command_buffer > _pending_commands;
		ptrdiff_t _submit_index = 0;

		const config _cfg;
	};
}
