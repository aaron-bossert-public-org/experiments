//
//#include <vulkan/sync/vulkan_resource.h>
//#include <vulkan/sync/vulkan_fence.h>
//
//using namespace igpu;
//void vulkan_resource::add_fence(const std::shared_ptr<vulkan_fence>& fence)
//{
//	for (auto& existing : _fences)
//	{
//		if (fence->cfg().queue == existing->cfg().queue)
//		{
//			// should be overflow proof, in the sense that if new wraps around existing submit id, it should still
//			// count as being older after subtraction and casting to an int (unless it is more than 2 billion ahead.
//			int diff = (int)(fence->cfg().submit_index - existing->cfg().submit_index);
//			if (diff > 0)
//			{
//				existing = fence;
//			}
//			return;
//		}
//	}
//
//	_fences.push_back(fence);
//}
//
//void vulkan_resource::wait_on_fences()
//{
//	for (auto& fence : _fences)
//	{
//		fence->wait();
//	}
//
//	_fences.clear();
//}
//
//vulkan_resource::~vulkan_resource()
//{
//	ASSERT_CONTEXT(_fences.size() == 0);
//}
