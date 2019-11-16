
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

#include <functional>
#include <list>

namespace igpu
{
	class vulkan_command_buffer;
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

		using command_builder_t =
			std::function< void( vulkan_command_buffer& ) >;

		static std::unique_ptr< vulkan_queue > make( const config& );

		const config& cfg() const;

		void one_time_command( const command_builder_t& );

		void one_time_command(
			uint32_t wait_count,
			const VkSemaphore* p_wait_semaphores,
			const VkPipelineStageFlags* p_wait_stages,
			const command_builder_t&,
			uint32_t signal_count,
			const VkSemaphore* p_signal_semaphores );

		VkQueue vk_queue() const;

		void free_completed_commands();

		~vulkan_queue();

	private:
		vulkan_queue(
			const config&,
			VkQueue queue,
			VkCommandPool command_pool );

	private:
		VkQueue _vk_queue = nullptr;
		VkCommandPool _command_pool = nullptr;
		std::list< std::shared_ptr< vulkan_command_buffer > > _pending_commands;
		ptrdiff_t _submit_index = 0;

		const config _cfg;
	};
}
