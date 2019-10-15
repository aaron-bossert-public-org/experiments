
#include <vulkan/buffer/vulkan_fence.h>

using namespace igpu;

std::unique_ptr < vulkan_fence > vulkan_fence::make(const config& cfg)
{
	return std::unique_ptr < vulkan_fence >
		(new vulkan_fence(cfg));
}

VkFence vulkan_fence::get() const
{
	return _fence;
}

bool vulkan_fence::is_ready(uint64_t wait_nanosecnods)
{
	VkResult res
		= 0 == wait_nanosecnods 
		? vkGetFenceStatus ( _cfg.device, _fence)
		: vkWaitForFences ( _cfg.device, 1, &_fence, true, wait_nanosecnods);

	return res == VK_SUCCESS;
}

void vulkan_fence::wait(uint64_t err_msg_nanosecnods)
{
	double seconds_waited = 0;
	while (false == is_ready(err_msg_nanosecnods))
	{
		seconds_waited += double(err_msg_nanosecnods) / (1000 * 1000 * 1000);
		LOG_CONTEXT(WARNING,
			"waiting for fence %f seconds",
			seconds_waited);
	}
}

vulkan_fence::~vulkan_fence()
{
	wait();

	vkDestroyFence(_cfg.device, _fence, nullptr);
}

vulkan_fence::vulkan_fence(const config& cfg)
	: _cfg(cfg)
	, _fence(
		[&cfg] {

			VkFence fence = nullptr;
			vkCreateFence(cfg.device, &cfg.info, nullptr, &fence);
			return fence;

		}())
{


}
