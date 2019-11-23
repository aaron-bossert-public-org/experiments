
#include "vulkan/manager/vulkan_managers.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_queue.h"
#include "vulkan/sync/vulkan_resource.h"
#include "vulkan/texture/vulkan_image.h"

using namespace igpu;

namespace
{
	template < typename T >
	void push_one_cb(
		const scoped_ptr< vulkan_queue > queue,
		T&& builder,
		uint32_t wait_count = 0,
		VkSemaphore* p_wait_semaphores = nullptr,
		VkPipelineStageFlags* p_wait_stages = nullptr,
		uint32_t signal_count = 0,
		VkSemaphore* p_signal_semaphores = nullptr )
	{
		vulkan_command_buffer command_buffer( {
			queue->command_pool(),
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		} );

		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkCommandBuffer vk_cmds = command_buffer.vk_cmds();

		vkBeginCommandBuffer( vk_cmds, &begin_info );
		builder( vk_cmds );
		vkEndCommandBuffer( vk_cmds );

		auto* p_commands = &command_buffer;
		queue->push_commands(
			wait_count,
			p_wait_semaphores,
			p_wait_stages,
			1,
			&p_commands,
			signal_count,
			p_signal_semaphores );
	}
}

void vulkan_managers::push_frame_job(
	const scoped_ptr< vulkan_queue > queue,
	const std::initializer_list< frame_job_barrier >& job_barriers,
	const std::function< void( VkCommandBuffer ) >& builder )
{
	_cfg.staging->start_recording_transfers();

	for ( const auto& job_barrier : job_barriers )
	{
		_cfg.staging->record_transfer( job_barrier.resource() );
	}

	_cfg.staging->push_pending_transfers();

	_cfg.barrier->submit_frame_job_barriers(
		queue,
		job_barriers.end() - job_barriers.begin(),
		job_barriers.begin() );

	push_one_cb( queue, builder );
}

const vulkan_managers::config& vulkan_managers::cfg() const
{
	return _cfg;
}

vulkan_managers::~vulkan_managers()
{}

std::unique_ptr< vulkan_managers > vulkan_managers::make( const config& cfg )
{
	if ( !cfg.barrier )
	{
		LOG_CRITICAL( "failed to create vulkan_barrier_manager" );
	}
	else if ( !cfg.queues )
	{
		LOG_CRITICAL( "failed to create vulkan_queue_manager" );
	}
	else if ( !cfg.staging )
	{
		LOG_CRITICAL( "failed to create vulkan_staging_manager" );
	}
	else
	{
		return std::unique_ptr< vulkan_managers >( new vulkan_managers( cfg ) );
	}

	return nullptr;
}

vulkan_managers::vulkan_managers( const config& cfg )
	: _cfg( cfg )
{}


frame_job_barrier::frame_job_barrier(
	vulkan_buffer* buffer,
	decorator decorators,
	VkPipelineStageFlagBits stage,
	VkAccessFlagBits access )
	: _resource( buffer )
	, _job_scope( { decorators, stage, access } )
{
	ASSERT_CONTEXT( _job_scope.validate() );
}

frame_job_barrier::frame_job_barrier(
	vulkan_image* image,
	VkImageLayout layout,
	decorator decorators,
	VkPipelineStageFlagBits stage,
	VkAccessFlagBits access )
	: _resource( image )
	, _layout( layout )
	, _job_scope( { decorators, stage, access } )
{
	ASSERT_CONTEXT( image->is_valid_layout( layout ) );
	ASSERT_CONTEXT( _job_scope.validate() );
}

vulkan_resource* frame_job_barrier::resource() const
{
	return _resource;
}

VkImageLayout frame_job_barrier::layout() const
{
	return _layout;
}

const vulkan_job_scope& frame_job_barrier::job_scope() const
{
	return _job_scope;
}
