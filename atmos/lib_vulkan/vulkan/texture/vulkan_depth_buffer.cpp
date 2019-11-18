
#include "vulkan/texture/vulkan_depth_buffer.h"

#include "vulkan/texture/vulkan_image_t.h"

using namespace igpu;

VkFormat igpu::to_vulkan_format( depth_format format )
{
	switch ( format )
	{
	case depth_format::D24S8:
		return VK_FORMAT_D24_UNORM_S8_UINT;
	case depth_format::D32:
		return VK_FORMAT_D32_SFLOAT;
	case depth_format::D16:
		return VK_FORMAT_D16_UNORM;
	};

	if ( is_valid( format ) )
	{
		LOG_CRITICAL( "unhandled format: %s", to_string( format ).data() );
	}
	else
	{
		LOG_CRITICAL( "invalid format: %d", format );
	}

	return VK_FORMAT_UNDEFINED;
}

std::unique_ptr< vulkan_depth_buffer > vulkan_depth_buffer::make(
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
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_IMAGE_TILING_OPTIMAL,
		cfg.vk.sample_count,
		cfg.vk.sharing_mode,
		VK_IMAGE_ASPECT_DEPTH_BIT,
		1 );

	return vulkan_image_target_t< vulkan_depth_buffer >::make( cfg, image_cfg );
}
