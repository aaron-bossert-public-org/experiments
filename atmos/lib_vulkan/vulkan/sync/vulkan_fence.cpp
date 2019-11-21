
#include "vulkan/sync/vulkan_fence.h"

#include "vulkan/sync/vulkan_queue.h"

using namespace igpu;

std::unique_ptr< vulkan_fence > vulkan_fence::make( const config& cfg )
{
	return std::unique_ptr< vulkan_fence >( new vulkan_fence( cfg ) );
}

const vulkan_fence::config& vulkan_fence::cfg() const
{
	return _cfg;
}

VkFence vulkan_fence::vk_fence() const
{
	return _fence;
}

bool vulkan_fence::is_ready( ptrdiff_t submit_index, uint64_t wait_nanosecnods )
{
	// if submit_index > _submit_index, but with int wrapping
	if ( 0 < submit_index - _submit_index )
	{
		return true;
	}

	VkResult res = 0 == wait_nanosecnods
		? vkGetFenceStatus( _cfg.device, _fence )
		: vkWaitForFences( _cfg.device, 1, &_fence, true, wait_nanosecnods );

	if ( res == VK_SUCCESS )
	{
		_submit_index = 0;
		return true;
	}

	return false;
}

void vulkan_fence::wait( ptrdiff_t submit_index, uint64_t err_msg_nanosecnods )
{
	double seconds_waited = 0;
	while ( false == is_ready( submit_index, err_msg_nanosecnods ) )
	{
		seconds_waited +=
			double( err_msg_nanosecnods ) / ( 1000 * 1000 * 1000 );
		LOG_CRITICAL( "waiting for fence %f seconds", seconds_waited );
	}
}

void vulkan_fence::on_submit( const vulkan_queue& queue )
{
	_submit_index = queue.submit_index();
}

ptrdiff_t vulkan_fence::submit_index() const
{
	return _submit_index;
}

vulkan_fence::~vulkan_fence()
{
	if ( _submit_index )
	{
		wait( _submit_index );
	}

	vkDestroyFence( _cfg.device, _fence, nullptr );
}

vulkan_fence::vulkan_fence( const config& cfg )
	: _cfg( cfg )
	, _fence( [&cfg] {
		VkFence fence = nullptr;
		vkCreateFence( cfg.device, &cfg.info, nullptr, &fence );
		return fence;
	}() )
{}
