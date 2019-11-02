
#include <vulkan/sync/vulkan_semaphore.h>

using namespace igpu;

VkSemaphore vulkan_semaphore::get() const
{
	return _semaphore;
}

std::unique_ptr<vulkan_semaphore> vulkan_semaphore::make(const config& cfg)
{
	VkSemaphore semaphore = nullptr;
	if (vkCreateSemaphore(cfg.device, &cfg.info, nullptr, &semaphore) != VK_SUCCESS)
	{
		return nullptr;
	}

	return std::unique_ptr<vulkan_semaphore>(new vulkan_semaphore(cfg, semaphore));
}

vulkan_semaphore::~vulkan_semaphore()
{
}

vulkan_semaphore::vulkan_semaphore(const config& cfg, VkSemaphore semaphore)
	: _cfg(cfg)
	, _semaphore(semaphore)
{
}
