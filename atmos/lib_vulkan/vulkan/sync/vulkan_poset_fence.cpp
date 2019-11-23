
#include "vulkan/sync/vulkan_poset_fence.h"

#include "vulkan/sync/vulkan_queue.h"

using namespace igpu;

std::unique_ptr< vulkan_poset_fence > vulkan_poset_fence::make(
	const config& cfg )
{
	return std::unique_ptr< vulkan_poset_fence >(
		new vulkan_poset_fence( cfg ) );
}

const vulkan_poset_fence::config& vulkan_poset_fence::cfg() const
{
	return _cfg;
}

VkFence vulkan_poset_fence::vk_fence() const
{
	return _vk_fence;
}


bool vulkan_poset_fence::is_ready( ptrdiff_t submit_index )
{
	// if submit_index > _submit_index, but with int wrapping
	if ( 0 < submit_index - _submit_index )
	{
		return true;
	}

	if ( VK_SUCCESS == vkGetFenceStatus( _cfg.device, _vk_fence ) )
	{
		// change submit index so all checks automatically pass until this is
		// submitted to a queue again
		_submit_index = 0;
		return true;
	}

	return false;
}

void vulkan_poset_fence::wait_or_skip( ptrdiff_t submit_index )
{
	if ( !is_ready( submit_index ) )
	{
		int seconds_waited = 0;
		while (
			vkWaitForFences( _cfg.device, 1, &_vk_fence, true, (uint64_t)1e9 ) )
		{
			++seconds_waited;

			LOG_CRITICAL( "waiting for fence %d seconds", seconds_waited );
		}
	}

	_submit_index = 0;
}

void vulkan_poset_fence::on_submit( const vulkan_queue& queue )
{
	_submit_index = queue.submit_index();
}

ptrdiff_t vulkan_poset_fence::submit_index() const
{
	return _submit_index;
}

vulkan_poset_fence::~vulkan_poset_fence()
{
	if ( _submit_index )
	{
		wait_or_skip( _submit_index );
	}

	vkDestroyFence( _cfg.device, _vk_fence, nullptr );
}

vulkan_poset_fence::vulkan_poset_fence( const config& cfg )
	: _cfg( cfg )
	, _vk_fence( [&cfg] {
		VkFence fence = nullptr;
		vkCreateFence( cfg.device, &cfg.info, nullptr, &fence );
		return fence;
	}() )
{}
