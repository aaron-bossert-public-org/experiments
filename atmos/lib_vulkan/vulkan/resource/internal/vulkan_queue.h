
#pragma once

// Vulkan implementation includes - begin
#include <vulkan/defines/vulkan_includes.h>
// Vulkan implementation includes - end

namespace igpu
{
	class vulkan_queue
	{
	public:

		struct config
		{
			VkQueueFamilyProperties properties;
			uint32_t queue_family_index = ~0U;
			VkQueue queue = nullptr;
		};

		vulkan_queue(const config&);

		const config& cfg() const;

		uint32_t batch_index() const;
		
		void batch_index(uint32_t);

	private:

		const config _cfg;

		VkCommandPool _command_pool;
		uint32_t _batch_index = ~0U;
	};
}
