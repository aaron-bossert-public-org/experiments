
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

#include <functional>
#include <list>

namespace igpu
{
	class vulkan_abandon_manager;
	class vulkan_command_buffer;
	class vulkan_command_pool;
	class vulkan_poset_fence;

	class vulkan_queue
	{
	public:
		struct config
		{
			VkDevice device;
			uint32_t family_index = ~0U;
		};

		using command_builder_t = std::function< void( VkCommandBuffer& ) >;

		static std::shared_ptr< vulkan_queue > make( const config& );


		const config& cfg() const;

		scoped_ptr< vulkan_command_pool > command_pool() const;

		ptrdiff_t submit_index() const;

		vulkan_abandon_manager& abandon_manager() const;

		void trigger_abandon();

		void submit_command( const vulkan_command_buffer& command_buffer );

		void submit_commands(
			uint32_t wait_count,
			const VkSemaphore* p_wait_semaphores,
			const VkPipelineStageFlags* p_wait_stages,
			uint32_t command_buffer_count,
			const vulkan_command_buffer* command_buffers,
			uint32_t signal_count,
			const VkSemaphore* p_signal_semaphores,
			std::shared_ptr< vulkan_poset_fence > fence = nullptr );

		VkResult submit_present( const VkPresentInfoKHR& present_info );

		~vulkan_queue();

	private:
		vulkan_queue( const config&, VkQueue );
		const config _cfg;
		VkQueue _vk_queue = nullptr;
		std::shared_ptr< vulkan_abandon_manager > _abandon_manager;
		std::shared_ptr< vulkan_command_pool > _command_pool;

		bool _trigger_abandon = false;
		ptrdiff_t _submit_index = 0;
	};
}
