
#include "vulkan/window/vulkan_back_buffer.h"

#include "vulkan/context/vulkan_context.h"
#include "vulkan/sync/vulkan_queue.h"
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

		return best_mode;
	}

	VkSwapchainKHR create_swap_chain(
		const vulkan_back_buffer::config& cfg,
		const VkSurfaceCapabilitiesKHR& surface_caps,
		const VkSurfaceFormatKHR& surface_format,
		uint32_t image_count )
	{
		VkPresentModeKHR present_mode = choose_swap_present_mode( cfg );

		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = cfg.back_buffer.surface;
		create_info.minImageCount = image_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageColorSpace = surface_format.colorSpace;
		create_info.imageExtent = {
			(uint32_t)cfg.color->cfg().res.x,
			(uint32_t)cfg.color->cfg().res.y,
		};
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		create_info.preTransform = surface_caps.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;

		uint32_t queue_family_indices[2] = {
			cfg.back_buffer.present_queue_family,
			cfg.back_buffer.graphics_queue_family,
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

	std::vector< VkImageView > create_image_views(
		const vulkan_back_buffer::config& cfg,
		const std::vector< VkImage >& swap_chain_images,
		VkFormat image_format )
	{
		std::vector< VkImageView > swap_chain_image_views;
		swap_chain_image_views.reserve( swap_chain_images.size() );

		for ( VkImage swap_chain_image : swap_chain_images )
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
			vkCreateImageView(
				cfg.vk.device,
				&view_info,
				nullptr,
				&image_view );
			swap_chain_image_views.push_back( image_view );
		}

		return swap_chain_image_views;
	}

	std::vector< VkFramebuffer > create_framebuffers(
		const vulkan_back_buffer::config cfg,
		const std::vector< VkImageView >& image_views,
		VkRenderPass render_pass )
	{
		std::vector< VkFramebuffer > framebuffers( image_views.size() );

		for ( size_t i = 0; i < image_views.size(); i++ )
		{
			std::array< VkImageView, 3 > attachments = {
				cfg.vk.color->gpu_object().vk_image_view(),
				cfg.vk.depth->gpu_object().vk_image_view(),
				image_views[i],
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
			vkCreateFramebuffer(
				cfg.vk.device,
				&framebuffer_info,
				nullptr,
				&framebuffers[i] );
		}

		return framebuffers;
	}

}

const vulkan_back_buffer::config& vulkan_back_buffer::cfg() const
{
	return _cfg;
}

void vulkan_back_buffer::end_raster() override
{
	vulkan_draw_target::end_raster();
	_cfg.vk.context->end_frame();
}

std::unique_ptr< vulkan_back_buffer > vulkan_back_buffer::make(
	const config& cfg )
{
	if ( !cfg.color )
	{
		LOG_CRITICAL( "render target is null" );
		return nullptr;
	}
	else if ( !cfg.depth )
	{
		LOG_CRITICAL( "depth target is null" );
		return nullptr;
	}

	vulkan_back_buffer::config back_buffer_cfg = {};
	VkSurfaceCapabilitiesKHR surface_caps = {};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		cfg.back_buffer.physical_device,
		cfg.back_buffer.surface,
		&surface_caps );

	VkSurfaceFormatKHR surface_format = {
		cfg.vk.color->gpu_object().cfg().image_info.format,
		cfg.back_buffer.color_space,
	};

	if ( !validate_swap_surface_format( cfg, surface_format ) )
	{
		return nullptr;
	}

	uint32_t image_count = surface_caps.minImageCount + 1;
	if ( surface_caps.maxImageCount > 0 &&
		 image_count > surface_caps.maxImageCount )
	{
		image_count = surface_caps.maxImageCount;
	}


	VkSwapchainKHR swap_chain =
		create_swap_chain( cfg, surface_caps, surface_format, image_count );
	auto images = create_images( cfg, swap_chain );
	auto image_views = create_image_views( cfg, images, surface_format.format );

	return std::unique_ptr< vulkan_back_buffer >(
		new vulkan_back_buffer( cfg, swap_chain, images, image_views ) );
}

VkSwapchainKHR vulkan_back_buffer::swap_chain() const
{
	return _swap_chain;
}


const std::vector< VkFramebuffer >& vulkan_back_buffer::framebuffers() const
{
	return _framebuffers;
}

vulkan_back_buffer::~vulkan_back_buffer()
{
	for ( auto framebuffer : _framebuffers )
	{
		vkDestroyFramebuffer( _cfg.vk.device, framebuffer, nullptr );
	}

	for ( auto image_view : _image_views )
	{
		vkDestroyImageView( _cfg.vk.device, image_view, nullptr );
	}

	vkDestroySwapchainKHR( _cfg.vk.device, _swap_chain, nullptr );
}

vulkan_back_buffer::vulkan_back_buffer(
	const config& cfg,
	VkSwapchainKHR swap_chain,
	const std::vector< VkImage >& images,
	const std::vector< VkImageView >& image_views )
	: vulkan_draw_target( cfg )
	, _cfg( cfg )
	, _swap_chain( swap_chain )
	, _images( images )
	, _image_views( image_views )
{
	_framebuffers = create_framebuffers( cfg, image_views, render_pass() );
}
