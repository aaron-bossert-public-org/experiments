
#include "vulkan/sync/vulkan_gpu_object.h"

#include "vulkan/sync/vulkan_fence.h"

using namespace igpu;
void vulkan_gpu_object::add_fence(
	const std::shared_ptr< vulkan_fence >& fence )
{
	auto& fences = object_state().fences;

	for ( auto& existing : fences )
	{
		if ( fence->cfg().queue == existing->cfg().queue )
		{
			// should be overflow proof, in the sense that if new wraps around
			// existing submit id, it should still count as being older after
			// subtraction (unless it is more than 2 billion ahead.)
			ptrdiff_t diff =
				fence->cfg().submit_index - existing->cfg().submit_index;
			if ( diff > 0 )
			{
				existing = fence;
			}
			return;
		}
	}

	fences.push_back( fence );
}

void vulkan_gpu_object::wait_on_fences()
{
	auto& fences = object_state().fences;

	for ( auto& fence : fences )
	{
		fence->wait();
	}

	fences.clear();
}

vulkan_gpu_object::state::~state()
{
	ASSERT_CONTEXT(
		fences.size() == 0,
		"resource was destroyed while gpu is still accessing it" );
}
