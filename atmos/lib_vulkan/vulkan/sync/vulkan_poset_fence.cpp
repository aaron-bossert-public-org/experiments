
#include "vulkan/sync/vulkan_poset_fence.h"

#include "vulkan/sync/vulkan_fence.h"
#include "vulkan/sync/vulkan_queue.h"

using namespace igpu;

bool vulkan_poset_fence::is_ready()
{
	if ( _queue && _submit_index )
	{
		return _queue->is_ready( _submit_index );
	}
	return true;
}

void vulkan_poset_fence::wait_or_skip()
{
	if ( _queue && _submit_index )
	{
		_queue->wait( _submit_index );
	}
}

vulkan_poset_fence vulkan_poset_fence::prev_submit( vulkan_queue* queue )
{
	ASSERT_CONTEXT( queue );
	vulkan_poset_fence f;
	f._queue = queue;
	f._submit_index = queue->submit_index();
	return f;
}

vulkan_poset_fence vulkan_poset_fence::next_submit( vulkan_queue* queue )
{
	ASSERT_CONTEXT( queue );
	vulkan_poset_fence f;
	f._queue = queue;
	f._submit_index = queue->submit_index() + 1;
	return f;
}

vulkan_poset_fence vulkan_poset_fence::past(
	vulkan_queue* queue,
	ptrdiff_t submit_index )
{
	ASSERT_CONTEXT( queue );
	ASSERT_CONTEXT( submit_index );
	vulkan_poset_fence f;
	f._queue = queue;
	f._submit_index = submit_index;
	return f;
}