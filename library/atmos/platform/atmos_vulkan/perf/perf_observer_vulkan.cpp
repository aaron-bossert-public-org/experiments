
#include "perf_observer_vulkan.h"
using namespace perf;
using namespace platform;


perf_observer::duration_t perf_observer::frame_time() const
{
	//VkQueryPoolCreateInfo createInfo = {};
	//createInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	//createInfo.pNext = nullptr;
	//createInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
	//createInfo.queryCount = 2;

	//VkResult res = vkCreateQueryPool(GContext.lDevice.device, &createInfo, nullptr, &rs.queryPool);
	//assert(res == VK_SUCCESS);
	return {};
}

perf_observer::~perf_observer()
{

}