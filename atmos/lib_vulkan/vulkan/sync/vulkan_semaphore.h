
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include <memory>

namespace igpu
{
	class vulkan_semaphore
	{
	public:
		struct config
		{
			VkDevice device = nullptr;
			VkSemaphoreCreateInfo info = {
				VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				nullptr,
				0,
			};
		};

		VkSemaphore vk_semaphore() const;

		static std::unique_ptr< vulkan_semaphore > make( const config& );

		~vulkan_semaphore();

	private:
		vulkan_semaphore( const config&, VkSemaphore );


	private:
		const config _cfg;
		const VkSemaphore _semaphore;
	};
}
