
#include <vulkan/window/vulkan_back_buffer.h>

#include <vulkan/buffer/vulkan_queue.h>
#include <framework/logging/log.h>

#include <array>

using namespace igpu;

namespace
{
	std::unique_ptr<vulkan_color_buffer> create_color_buffer(
		const vulkan_back_buffer::config& cfg)
	{
		return vulkan_color_buffer::make({
			"back buffer color",
			cfg.color_format,
			cfg.sampler,
			cfg.res,
			cfg.physical_device,
			cfg.device,
			cfg.sample_count,
			VK_SHARING_MODE_EXCLUSIVE });
	}

	std::unique_ptr<vulkan_depth_buffer> create_depth_buffer(
		const vulkan_back_buffer::config& cfg)
	{
		return vulkan_depth_buffer::make({
			"back buffer depth",
			cfg.depth_format,
			cfg.sampler,
			cfg.res,
			cfg.physical_device,
			cfg.device,
			cfg.sample_count,
			VK_SHARING_MODE_EXCLUSIVE });
	}

	bool validate_swap_surface_format(
		const vulkan_back_buffer::config& cfg,
		const VkSurfaceFormatKHR& surface_format)
	{
		uint32_t format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(cfg.physical_device, cfg.surface, &format_count, nullptr);

		std::vector<VkSurfaceFormatKHR> formats(format_count);
		if (format_count != 0)
		{
			vkGetPhysicalDeviceSurfaceFormatsKHR(cfg.physical_device, cfg.surface, &format_count, formats.data());
		}

		if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
		{
			LOG_WARNING("there is only one swap surface format and its color format is undefined, using it anyway");
			return true;
		}

		bool found_color = false;

		for (const auto& format : formats)
		{
			if (format.format == surface_format.format)
			{
				found_color = true;
				if (format.colorSpace == surface_format.colorSpace)
				{
					return true;
				}
			}
		}

		if (!found_color)
		{
			LOG_CRITICAL("could not find support for surface format %s", to_string(cfg.color_format).data());
		}
		else
		{
			LOG_CRITICAL("could not find support for surface color space %d", cfg.color_space);
		}

		return false;
	}

	VkPresentModeKHR choose_swap_present_mode(
		const vulkan_back_buffer::config& cfg)
	{
		uint32_t present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(cfg.physical_device, cfg.surface, &present_mode_count, nullptr);

		std::vector<VkPresentModeKHR> present_modes(present_mode_count);
		if (present_mode_count != 0)
		{
			vkGetPhysicalDeviceSurfacePresentModesKHR(cfg.physical_device, cfg.surface, &present_mode_count, present_modes.data());
		}

		VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR;

		for (const auto& present_mode : present_modes)
		{
			if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return present_mode;
			}
			else if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
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
		uint32_t image_count)
	{
		VkPresentModeKHR present_mode = choose_swap_present_mode(cfg);

		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = cfg.surface;
		create_info.minImageCount = image_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageColorSpace = surface_format.colorSpace;
		create_info.imageExtent = { (uint32_t)cfg.res.x, (uint32_t)cfg.res.y };
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		create_info.preTransform = surface_caps.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;

		uint32_t queue_family_indices[2] = {
			cfg.present_queue_family,
			cfg.graphics_queue_family
		};

		if (queue_family_indices[0] != queue_family_indices[1])
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
		vkCreateSwapchainKHR(cfg.device, &create_info, nullptr, &swap_chain);
		return swap_chain;
	}

	std::vector<VkImage> create_images(
		const vulkan_back_buffer::config& cfg,
		VkSwapchainKHR swap_chain)
	{
		uint32_t image_count = 0;
		vkGetSwapchainImagesKHR(cfg.device, swap_chain, &image_count, nullptr);
		std::vector<VkImage>images(image_count);
		vkGetSwapchainImagesKHR(cfg.device, swap_chain, &image_count, images.data());
		return images;
	}

	std::vector<VkImageView> create_image_views(
		const vulkan_back_buffer::config& cfg, 
		const std::vector<VkImage>& swap_chain_images, 
		VkFormat image_format)
	{
		std::vector<VkImageView> swap_chain_image_views;
		swap_chain_image_views.reserve(swap_chain_images.size());

		for (VkImage swap_chain_image : swap_chain_images)
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
			vkCreateImageView(cfg.device, &view_info, nullptr, &image_view);
			swap_chain_image_views.push_back(image_view);
		}

		return swap_chain_image_views;
	}

	VkRenderPass create_render_pass(
		const vulkan_back_buffer::config& cfg,
		VkFormat color_format,
		VkFormat depth_format)
	{
		VkAttachmentDescription color_attachment = {};
		color_attachment.format = color_format;
		color_attachment.samples = cfg.sample_count;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depth_attachment = {};
		depth_attachment.format = depth_format;
		depth_attachment.samples = cfg.sample_count;
		depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription color_attachment_resolve = {};
		color_attachment_resolve.format = color_format;
		color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depth_attachment_ref = {};
		depth_attachment_ref.attachment = 1;
		depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference color_attachment_resolve_ref = {};
		color_attachment_resolve_ref.attachment = 2;
		color_attachment_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;
		subpass.pDepthStencilAttachment = &depth_attachment_ref;
		subpass.pResolveAttachments = &color_attachment_resolve_ref;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 3> attachments = { color_attachment, depth_attachment, color_attachment_resolve };
		VkRenderPassCreateInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
		render_pass_info.pAttachments = attachments.data();
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;
		render_pass_info.dependencyCount = 1;
		render_pass_info.pDependencies = &dependency;
		
		VkRenderPass render_pass = nullptr;
		vkCreateRenderPass(cfg.device, &render_pass_info, nullptr, &render_pass);
		return render_pass;
	}

	std::vector<VkFramebuffer> create_framebuffers(
		const vulkan_back_buffer::config cfg, 
		VkImageView color_image_view,
		VkImageView depth_image_view,
		const std::vector<VkImageView>& image_views,
		VkRenderPass render_pass)
	{
		std::vector<VkFramebuffer> framebuffers(image_views.size());

		for (size_t i = 0; i < image_views.size(); i++)
		{
			std::array<VkImageView, 3> attachments = {
				color_image_view,
				depth_image_view,
				image_views[i]
			};

			VkFramebufferCreateInfo framebuffer_info = {};
			framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_info.renderPass = render_pass;
			framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebuffer_info.pAttachments = attachments.data();
			framebuffer_info.width = cfg.res.x;
			framebuffer_info.height = cfg.res.y;
			framebuffer_info.layers = 1;
			vkCreateFramebuffer(cfg.device, &framebuffer_info, nullptr, &framebuffers[i]);
		}

		return framebuffers;
	}

}

const vulkan_back_buffer::config& vulkan_back_buffer::cfg() const
{
	return _cfg;
}

const vulkan_color_buffer& vulkan_back_buffer::color() const
{
	return *_color;
}

const vulkan_depth_buffer& vulkan_back_buffer::depth() const
{
	return *_depth;
}

std::unique_ptr<vulkan_back_buffer> vulkan_back_buffer::make(const config& cfg)
{
	auto color = create_color_buffer(cfg);
	auto depth = create_depth_buffer(cfg);

	if (!color || !depth)
	{
		return nullptr;
	}

	vulkan_back_buffer::config back_buffer_cfg = {};
	VkSurfaceCapabilitiesKHR surface_caps = {};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(cfg.physical_device, cfg.surface, &surface_caps);

	VkSurfaceFormatKHR surface_format = {
		color->image_cfg().info.format,
		cfg.color_space,
	};

	if (!validate_swap_surface_format(cfg, surface_format))
	{
		return nullptr;
	}

	uint32_t image_count = surface_caps.minImageCount + 1;
	if (surface_caps.maxImageCount > 0 && image_count > surface_caps.maxImageCount)
	{
		image_count = surface_caps.maxImageCount;
	}


	VkSwapchainKHR swap_chain = create_swap_chain(cfg, surface_caps, surface_format, image_count);
	auto images = create_images(cfg, swap_chain);
	auto image_views = create_image_views(cfg, images, surface_format.format);
	VkRenderPass render_pass = create_render_pass(
		cfg,
		color->image_cfg().info.format,
		depth->image_cfg().info.format);
	
	image_count = (uint32_t)images.size();

	auto framebuffers = create_framebuffers(
		cfg, 
		color->image_view(),
		depth->image_view(),
		image_views,
		render_pass);

	return std::unique_ptr<vulkan_back_buffer>(
		new vulkan_back_buffer(
			cfg,
			swap_chain,
			render_pass,
			images,
			image_views,
			framebuffers,
			std::move(color),
			std::move(depth)));
}

VkSwapchainKHR vulkan_back_buffer::swap_chain() const
{
	return _swap_chain;
}

VkRenderPass vulkan_back_buffer::render_pass() const
{
	return _render_pass;
}

const std::vector<VkFramebuffer>& vulkan_back_buffer::framebuffers() const
{
	return _framebuffers;
}

vulkan_back_buffer::~vulkan_back_buffer()
{
	for (auto framebuffer : _framebuffers)
	{
		vkDestroyFramebuffer(_cfg.device, framebuffer, nullptr);
	}

	for (auto image_view : _image_views)
	{
		vkDestroyImageView(_cfg.device, image_view, nullptr);
	}

	vkDestroySwapchainKHR(_cfg.device, _swap_chain, nullptr);

	vkDestroyRenderPass(_cfg.device, _render_pass, nullptr);


}

vulkan_back_buffer::vulkan_back_buffer(
	const config& cfg,
	VkSwapchainKHR swap_chain,
	VkRenderPass render_pass,
	const std::vector<VkImage>& images,
	const std::vector<VkImageView>& image_views,
	const std::vector<VkFramebuffer>& framebuffers,
	std::unique_ptr<vulkan_color_buffer> color,
	std::unique_ptr<vulkan_depth_buffer> depth)
: _cfg(cfg)
, _swap_chain(swap_chain)
, _render_pass(render_pass)
, _images(images)
, _image_views(image_views)
, _framebuffers(framebuffers)
, _color (std::move(color))
, _depth(std::move(depth))
{
}
