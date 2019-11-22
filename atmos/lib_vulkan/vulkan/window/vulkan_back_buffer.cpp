#include "vulkan/window/vulkan_back_buffer.h"

#include "vulkan/context/vulkan_context.h"
#include "vulkan/manager/vulkan_abandon_manager.h"
#include "vulkan/manager/vulkan_queue_manager.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_command_pool.h"
#include "vulkan/sync/vulkan_poset_fence.h"
#include "vulkan/sync/vulkan_queue.h"
#include "vulkan/sync/vulkan_semaphore.h"
#include "vulkan/texture/vulkan_image.h"

#include "framework/logging/log.h"

#include <array>

using namespace igpu;

namespace
{
	bool validate_swap_surface_format(
		const vulkan_back_buffer::config& cfg,
		const VkSurfaceFormatKHR& surface_format )
	{
		uint32_t format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			cfg.back_buffer.physical_device,
			cfg.back_buffer.surface,
			&format_count,
			nullptr );

		std::vector< VkSurfaceFormatKHR > formats( format_count );
		if ( format_count != 0 )
		{
			vkGetPhysicalDeviceSurfaceFormatsKHR(
				cfg.back_buffer.physical_device,
				cfg.back_buffer.surface,
				&format_count,
				formats.data() );
		}

		if ( formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED )
		{
			LOG_WARNING(
				"there is only one swap surface format and its color format is "
				"undefined, using it anyway" );
			return true;
		}

		bool found_color = false;

		for ( const auto& format : formats )
		{
			if ( format.format == surface_format.format )
			{
				found_color = true;
				if ( format.colorSpace == surface_format.colorSpace )
				{
					return true;
				}
			}
		}

		if ( !found_color )
		{
			LOG_CRITICAL(
				"could not find support for surface format %s",
				to_string( cfg.color->cfg().format ).data() );
		}
		else
		{
			LOG_CRITICAL(
				"could not find support for surface color space %d",
				cfg.back_buffer.color_space );
		}

		return false;
	}

	VkPresentModeKHR choose_swap_present_mode(
		const vulkan_back_buffer::config& cfg )
	{
		uint32_t present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			cfg.back_buffer.physical_device,
			cfg.back_buffer.surface,
			&present_mode_count,
			nullptr );

		std::vector< VkPresentModeKHR > present_modes( present_mode_count );
		if ( present_mode_count != 0 )
		{
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				cfg.back_buffer.physical_device,
				cfg.back_buffer.surface,
				&present_mode_count,
				present_modes.data() );
		}

		VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR;

		if ( !cfg.back_buffer.vsync )
		{
			for ( const auto& present_mode : present_modes )
			{
				if ( present_mode == VK_PRESENT_MODE_MAILBOX_KHR )
				{
					return present_mode;
				}
				else if ( present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR )
				{
					best_mode = present_mode;
				}
			}
		}

		return best_mode;
	}

	VkSwapchainKHR create_swap_chain(
		const vulkan_back_buffer::config& cfg,
		const VkSurfaceFormatKHR& surface_format )
	{
		VkPresentModeKHR present_mode = choose_swap_present_mode( cfg );

		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = cfg.back_buffer.surface;
		create_info.minImageCount = (uint32_t)cfg.vk.swap_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageColorSpace = surface_format.colorSpace;
		create_info.imageExtent = {
			(uint32_t)cfg.color->cfg().res.x,
			(uint32_t)cfg.color->cfg().res.y,
		};
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;
		create_info.preTransform =
			cfg.back_buffer.surface_caps.currentTransform;

		uint32_t queue_family_indices[2] = {
			cfg.vk.queue_manager->cfg().graphics_queue->cfg().family_index,
			cfg.vk.queue_manager->cfg().present_queue->cfg().family_index,
		};

		if ( queue_family_indices[0] != queue_family_indices[1] )
		{
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else
		{
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		VkSwapchainKHR swap_chain = nullptr;
		vkCreateSwapchainKHR(
			cfg.vk.device,
			&create_info,
			nullptr,
			&swap_chain );
		return swap_chain;
	}

	std::vector< VkImage > create_images(
		const vulkan_back_buffer::config& cfg,
		VkSwapchainKHR swap_chain )
	{
		uint32_t image_count = 0;
		vkGetSwapchainImagesKHR(
			cfg.vk.device,
			swap_chain,
			&image_count,
			nullptr );
		std::vector< VkImage > images( image_count );
		vkGetSwapchainImagesKHR(
			cfg.vk.device,
			swap_chain,
			&image_count,
			images.data() );
		return images;
	}

	VkImageView create_image_views(
		const vulkan_back_buffer::config& cfg,
		VkImage swap_chain_image,
		VkFormat image_format )
	{
		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = swap_chain_image;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = image_format;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		VkImageView image_view;
		vkCreateImageView( cfg.vk.device, &view_info, nullptr, &image_view );
		return image_view;
	}

	VkFramebuffer create_framebuffer(
		const vulkan_back_buffer::config cfg,
		VkImageView image_view,
		VkRenderPass render_pass )
	{
		std::array< VkImageView, 3 > attachments = {
			cfg.vk.color->gpu_object().vk_image_view(),
			cfg.vk.depth->gpu_object().vk_image_view(),
			image_view,
		};

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = render_pass;
		framebuffer_info.attachmentCount =
			static_cast< uint32_t >( attachments.size() );
		framebuffer_info.pAttachments = attachments.data();
		framebuffer_info.width = cfg.color->cfg().res.x,
		framebuffer_info.height = cfg.color->cfg().res.y,
		framebuffer_info.layers = 1;

		VkFramebuffer framebuffer = nullptr;
		vkCreateFramebuffer(
			cfg.vk.device,
			&framebuffer_info,
			nullptr,
			&framebuffer );
		return framebuffer;
	}
}

const vulkan_back_buffer::config& vulkan_back_buffer::cfg() const
{
	return _cfg;
}

void vulkan_back_buffer::begin_raster()
{
	swap_state& frame_state = frame_swap_state();

	if ( !frame_state.raster_fence )
	{
		frame_state.raster_fence = vulkan_poset_fence::make( {
			_cfg.vk.device,
		} );
	}

	VkCommandBuffer vk_cmds = frame_state.raster_cmds->vk_cmds();
	VkCommandBufferBeginInfo begin_info = {};

	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer( vk_cmds, &begin_info );

	VkRenderPassBeginInfo render_pass_info = {
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
	};

	render_pass_info.renderPass = render_pass();
	render_pass_info.framebuffer = frame_state.framebuffer;
	render_pass_info.renderArea.offset = {};
	render_pass_info.renderArea.extent = {
		(uint32_t)res().x,
		(uint32_t)res().y,
	};

	std::array< VkClearValue, 2 > clear_values = {};
	clear_values[0].color = {
		0.0f,
		0.0f,
		0.0f,
		1.0f,
	};
	clear_values[1].depthStencil = {
		1.0f,
		0,
	};

	render_pass_info.clearValueCount = (uint32_t)clear_values.size();
	render_pass_info.pClearValues = clear_values.data();
	vkCmdBeginRenderPass(
		vk_cmds,
		&render_pass_info,
		VK_SUBPASS_CONTENTS_INLINE );
}

scoped_ptr< vulkan_command_buffer > vulkan_back_buffer::raster_cmds()
{
	return _st.swap_states[_st.swap_index].raster_cmds;
}

scoped_ptr< vulkan_poset_fence > vulkan_back_buffer::raster_fence() const
{
	return _st.swap_states[_st.swap_index].raster_fence;
}

void vulkan_back_buffer::end_raster()
{
	switch ( do_end_raster() )
	{
	case VK_SUBOPTIMAL_KHR:
		LOG_WARNING( "vkAcquireNextImageKHR: VK_SUBOPTIMAL_KHR" );
		[[fallthrough]];
	case VK_ERROR_OUT_OF_DATE_KHR:
		_cfg.back_buffer.context->back_buffer_out_of_date();
	}
}

VkResult vulkan_back_buffer::do_end_raster()
{
	uint32_t image_index = 0;
	swap_state& frame_state = frame_swap_state();

	VkCommandBuffer vk_cmds = frame_state.raster_cmds->vk_cmds();
	VkSemaphore aquire_sem = frame_state.aquire_sem->vk_semaphore();
	VkSemaphore raster_sem = frame_state.aquire_sem->vk_semaphore();
	VkPipelineStageFlags aquire_stage =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	vkCmdEndRenderPass( vk_cmds );
	vkEndCommandBuffer( vk_cmds );

	if ( VkResult result = vkAcquireNextImageKHR(
			 _cfg.vk.device,
			 _st.swap_chain,
			 std::numeric_limits< uint64_t >::max(),
			 aquire_sem,
			 VK_NULL_HANDLE,
			 &image_index ) )
	{
		return result;
	}

	// after triggering abandon, the next submit will generate a fence if one is
	// not provided. Trigger abandon right before the graphics submit since it
	// always provides a fence.
	for ( const auto& queue : _cfg.vk.queue_manager->cfg().queue_family_table )
	{
		if ( queue )
		{
			queue->trigger_abandon();
		}
	}

	const auto& raster_queue = vulkan_draw_target::raster_queue();
	raster_queue->submit_commands(
		1,
		&aquire_sem,
		&aquire_stage,
		1,
		frame_state.raster_cmds.get(),
		1,
		&raster_sem,
		frame_state.raster_fence );

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &raster_sem;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &_st.swap_chain;
	present_info.pImageIndices = &image_index;


	VkResult present_errors =
		_cfg.vk.queue_manager->cfg().present_queue->submit_present(
			present_info );

	if ( present_errors )
	{
		return present_errors;
	}

	_st.swap_index = ( _st.swap_index + 1 ) % _cfg.vk.swap_count;

	swap_state& next_frame_state = frame_swap_state();

	if ( const auto& fence = next_frame_state.raster_fence )
	{
		fence->wait_or_skip( fence->submit_index() );
		VkFence vk_fence = fence->vk_fence();
		vkResetFences( _cfg.vk.device, 1, &vk_fence );
		vkResetCommandBuffer(
			next_frame_state.raster_cmds->vk_cmds(),
			(VkCommandBufferResetFlags)0 );
	}

	return VK_SUCCESS;
}

size_t vulkan_back_buffer::swap_index() const
{
	return _st.swap_index;
}

VkSwapchainKHR vulkan_back_buffer::swap_chain() const
{
	return _st.swap_chain;
}

VkFramebuffer vulkan_back_buffer::framebuffer( size_t swap_index ) const
{
	return _st.swap_states[swap_index].framebuffer;
}

std::unique_ptr< vulkan_back_buffer > vulkan_back_buffer::make(
	const config& cfg )
{
	state st = {};
	VkSurfaceFormatKHR surface_format = {
		cfg.vk.color->gpu_object().cfg().image_info.format,
		cfg.back_buffer.color_space,
	};

	st.command_pool = vulkan_command_pool::make( {
		cfg.vk.queue_manager->cfg().graphics_queue,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
	} );

	if ( validate_swap_surface_format( cfg, surface_format ) )
	{
		st.swap_chain = create_swap_chain( cfg, surface_format );
	}

	if ( !cfg.vk.color )
	{
		LOG_CRITICAL( "render target is null" );
	}
	else if ( !cfg.vk.depth )
	{
		LOG_CRITICAL( "depth target is null" );
	}
	else if ( !st.command_pool )
	{
		LOG_CRITICAL( "command_pool is null" );
	}
	else if ( !st.swap_chain )
	{
		LOG_CRITICAL( "swap_chain is null" );
	}
	else
	{
		st.swap_states.resize( cfg.vk.swap_count );
		auto images = create_images( cfg, st.swap_chain );

		for ( size_t i = 0; i < cfg.vk.swap_count; ++i )
		{
			auto& ss = st.swap_states[i];
			ss.image = images[i];
			ss.image_view =
				create_image_views( cfg, ss.image, surface_format.format );

			ss.aquire_sem = vulkan_semaphore::make( { cfg.vk.device } );
			ss.raster_sem = vulkan_semaphore::make( { cfg.vk.device } );
			ss.raster_cmds.reset( new vulkan_command_buffer( {
				st.command_pool,
				VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			} ) );
		}

		return std::unique_ptr< vulkan_back_buffer >(
			new vulkan_back_buffer( cfg, st ) );
	}

	return nullptr;
}

vulkan_back_buffer::~vulkan_back_buffer()
{
	vkDeviceWaitIdle( _cfg.vk.device );

	for ( auto& ss : _st.swap_states )
	{
		vkDestroyFramebuffer( _cfg.vk.device, ss.framebuffer, nullptr );
		vkDestroyImageView( _cfg.vk.device, ss.image_view, nullptr );
	}

	vkDestroySwapchainKHR( _cfg.vk.device, _st.swap_chain, nullptr );
}

vulkan_back_buffer::vulkan_back_buffer( const config& cfg, const state& st )
	: vulkan_draw_target( cfg )
	, _cfg( cfg )
	, _st( st )
{
	VkRenderPass rp = render_pass();

	for ( auto& ss : _st.swap_states )
	{
		ss.framebuffer = create_framebuffer( cfg, ss.image_view, rp );
	}
}

vulkan_back_buffer::swap_state& vulkan_back_buffer::frame_swap_state()
{
	return _st.swap_states[_st.swap_index];
}
