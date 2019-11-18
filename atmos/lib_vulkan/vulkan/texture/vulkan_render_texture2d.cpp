
#include "vulkan/texture/vulkan_render_texture2d.h"

#include "vulkan/texture/vulkan_image_t.h"
#include "vulkan/texture/vulkan_render_buffer.h"

using namespace igpu;

std::unique_ptr< vulkan_render_texture2d > vulkan_render_texture2d::make(
	const config& cfg )
{
	VkFormat vulkan_format = to_vulkan_format( cfg.format );

	if ( VK_FORMAT_UNDEFINED == vulkan_format )
	{
		return nullptr;
	}

	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties(
		cfg.vk.physical_device,
		vulkan_format,
		&props );

	if ( 0 ==
		 ( props.optimalTilingFeatures &
		   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) )
	{
		LOG_CRITICAL(
			"%s not supported as a depth attachment format",
			to_string( cfg.format ).data() );
		return false;
	}

	vulkan_image::config image_cfg = to_vulkan_image_info(
		cfg,
		memory_type::WRITE_COMBINED,
		cfg.res,
		vulkan_format,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_IMAGE_TILING_OPTIMAL,
		cfg.vk.sample_count,
		cfg.vk.sharing_mode,
		VK_IMAGE_ASPECT_COLOR_BIT,
		1 );

	return vulkan_image_target_t< vulkan_render_texture2d >::make(
		cfg,
		image_cfg );
}
