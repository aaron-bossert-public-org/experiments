
#include "vulkan/sync/vulkan_fence.h"

#include "vulkan/sync/vulkan_queue.h"

using namespace igpu;

std::unique_ptr< vulkan_fence > vulkan_fence::make(
	const config& cfg,
	ptrdiff_t submit_index )
{
	if ( !cfg.queue )
	{
		LOG_CRITICAL( "queue is null" );
	}
	else if ( !submit_index )
	{
		LOG_CRITICAL( "submit index is zero" );
	}
	else
	{
		VkFence vk_fence = nullptr;
		vkCreateFence( cfg.queue->cfg().device, &cfg.info, nullptr, &vk_fence );

		if ( !vk_fence )
		{
			LOG_CRITICAL( "failed to create VkFence" );
		}
		else
		{
			return std::unique_ptr< vulkan_fence >(
				new vulkan_fence( cfg, vk_fence, submit_index ) );
		}
	}

	return nullptr;
}

const vulkan_fence::config& vulkan_fence::cfg() const
{
	return _cfg;
}

VkFence vulkan_fence::vk_fence() const
{
	return _vk_fence;
}

ptrdiff_t vulkan_fence::submit_index() const
{
	return _submit_index;
}

void vulkan_fence::re_submit( ptrdiff_t submit_index )
{
	_submit_index = submit_index;
	vkResetFences( _device, 1, &_vk_fence );
}

bool vulkan_fence::is_ready() const
{
	return VK_SUCCESS == vkGetFenceStatus( _device, _vk_fence );
}

void vulkan_fence::wait() const
{
	int seconds_waited = 0;
	while ( vkWaitForFences( _device, 1, &_vk_fence, true, (uint64_t)1e9 ) )
	{
		++seconds_waited;

		LOG_CRITICAL( "waiting for fence %d seconds", seconds_waited );
	}
}

vulkan_fence::~vulkan_fence()
{
	vkDestroyFence( _device, _vk_fence, nullptr );
}

vulkan_fence::vulkan_fence(
	const config& cfg,
	VkFence vk_fence,
	ptrdiff_t submit_index )
	: _cfg( cfg )
	, _device( _cfg.queue->cfg().device )
	, _vk_fence( vk_fence )
	, _submit_index( submit_index )
{}
