
#include "vulkan/texture/vulkan_depth_texture2d.h"

#include "vulkan/texture/vulkan_depth_buffer.h"
#include "vulkan/texture/vulkan_image_t.h"

using namespace igpu;

std::unique_ptr< vulkan_depth_texture2d > vulkan_depth_texture2d::make(
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
		cfg.res,
		vulkan_format,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
			VK_IMAGE_USAGE_SAMPLED_BIT,
		cfg.vk.sample_count,
		cfg.vk.sharing_mode,
		VK_IMAGE_ASPECT_DEPTH_BIT,
		1 );

	return vulkan_image_target_t< vulkan_depth_texture2d >::make(
		cfg,
		image_cfg );
}
