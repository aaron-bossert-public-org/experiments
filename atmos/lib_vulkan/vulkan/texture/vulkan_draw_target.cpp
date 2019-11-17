
#include "vulkan/texture/vulkan_draw_target.h"

#include "vulkan/context/vulkan_abandon_manager.h"
#include "vulkan/sync/vulkan_barrier_manager.h"
#include "vulkan/sync/vulkan_synchronization.h"

#include "framework/logging/log.h"

#include <array>

using namespace igpu;

namespace
{
	VkRenderPass create_render_pass(
		const vulkan_draw_target::config& cfg,
		uint32_t* out_raster_sub_pass )
	{
		VkAttachmentDescription color_attachment = {};
		color_attachment.format = cfg.vk.color->cfg().vk.format;
		color_attachment.samples = cfg.vk.color->cfg().vk.sample_count;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depth_attachment = {};
		depth_attachment.format = cfg.vk.depth->cfg().vk.format;
		depth_attachment.samples = cfg.vk.depth->cfg().vk.sample_count;
		depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachment.finalLayout =
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription color_attachment_resolve = {};
		color_attachment_resolve.format = color_attachment.format;
		color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment_resolve.stencilLoadOp =
			VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment_resolve.stencilStoreOp =
			VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depth_attachment_ref = {};
		depth_attachment_ref.attachment = 1;
		depth_attachment_ref.layout =
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference color_attachment_resolve_ref = {};
		color_attachment_resolve_ref.attachment = 2;
		color_attachment_resolve_ref.layout =
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;
		subpass.pDepthStencilAttachment = &depth_attachment_ref;
		subpass.pResolveAttachments = &color_attachment_resolve_ref;

		VkSubpassDependency dependency = {};
		dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::array< VkAttachmentDescription, 3 > attachments = {
			color_attachment,
			depth_attachment,
			color_attachment_resolve,
		};

		VkRenderPassCreateInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount =
			static_cast< uint32_t >( attachments.size() );
		render_pass_info.pAttachments = attachments.data();
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;
		render_pass_info.dependencyCount = 1;
		render_pass_info.pDependencies = &dependency;

		*out_raster_sub_pass = 0;

		VkRenderPass render_pass = nullptr;
		vkCreateRenderPass(
			cfg.vk.device,
			&render_pass_info,
			nullptr,
			&render_pass );
		return render_pass;
	}
}

const vulkan_draw_target::config& vulkan_draw_target::cfg() const
{
	return _cfg;
}

VkRenderPass vulkan_draw_target::render_pass() const
{
	return _state.render_pass;
}

uint32_t vulkan_draw_target::raster_sub_pass() const
{
	return _state.raster_sub_pass;
}

std::unique_ptr< vulkan_draw_target > vulkan_draw_target::make(
	const config& cfg )
{
	if ( !cfg.color && !cfg.depth )
	{
		LOG_CRITICAL( "color and depth cannot both be null" );
	}
	else if ( !cfg.color != !cfg.vk.color )
	{
		LOG_CRITICAL( "failed to cast color target to vulkan color target" );
	}
	else if ( !cfg.depth != !cfg.vk.depth )
	{
		LOG_CRITICAL( "failed to cast depth target to vulkan depth target" );
	}
	else
	{
		LOG_CRITICAL( "render to offscreen draw targets not yet implemented " );

		// return std::unique_ptr< vulkan_draw_target >(
		//	new vulkan_draw_target( cfg ) );
	}

	return nullptr;
}

vulkan_draw_target::~vulkan_draw_target()
{
	vkDestroyRenderPass( _cfg.vk.device, _state.render_pass, nullptr );
}

vulkan_draw_target::vulkan_draw_target( const config& cfg )
	: _cfg( cfg )
	, _state( [&cfg]() {
		state st = {};
		st.render_pass = create_render_pass( cfg, &st.raster_sub_pass );
		return st;
	}() )
{}
