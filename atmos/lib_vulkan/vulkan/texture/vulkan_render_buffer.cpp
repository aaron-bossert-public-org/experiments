
#include "vulkan/texture/vulkan_render_buffer.h"

#include "vulkan/texture/vulkan_image_t.h"

using namespace igpu;

VkFormat igpu::to_vulkan_format( color_format format )
{
	switch ( format )
	{
	case color_format::INT_R8G8B8A8:
		return VK_FORMAT_B8G8R8A8_UNORM;
	case color_format::INT_R5G6B5:
		return VK_FORMAT_R5G6B5_UNORM_PACK16;
	case color_format::INT_R16:
		return VK_FORMAT_R16_UNORM;
	case color_format::FLOAT_R16:
		return VK_FORMAT_R16_SFLOAT;
	case color_format::FLOAT_R16G16:
		return VK_FORMAT_R16G16_SFLOAT;
	case color_format::FLOAT_R32:
		return VK_FORMAT_R32_SFLOAT;
	case color_format::FLOAT_R32G32:
		return VK_FORMAT_R32G32_SFLOAT;
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

std::unique_ptr< vulkan_render_buffer > vulkan_render_buffer::make(
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
		   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT ) )
	{
		LOG_CRITICAL(
			"%s not supported as a color attachment format",
			to_string( cfg.format ).data() );
		return false;
	}

	vulkan_image::config image_cfg = to_vulkan_image_info(
		cfg,
		cfg.res,
		vulkan_format,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		cfg.vk.sample_count,
		cfg.vk.sharing_mode,
		VK_IMAGE_ASPECT_COLOR_BIT,
		1 );

	return vulkan_image_target_t<
		vulkan_render_buffer >::make( cfg, image_cfg );
}
