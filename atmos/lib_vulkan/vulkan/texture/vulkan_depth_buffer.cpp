#include <vulkan/texture/vulkan_depth_buffer.h>

using namespace igpu;

VkFormat igpu::to_vulkan_format(depth_format format)
{
	switch (format)
	{
	case depth_format::D24S8:
		return VK_FORMAT_D24_UNORM_S8_UINT;
	case depth_format::D32:
		return VK_FORMAT_D32_SFLOAT;
	case depth_format::D16:
		return VK_FORMAT_D16_UNORM;
	};

	if (is_valid(format))
	{
		LOG_CRITICAL("unhandled format: %s", to_string(format).data());
	}
	else
	{
		LOG_CRITICAL("invalid format: %d", format);
	}

	return VK_FORMAT_UNDEFINED;
}

const vulkan_depth_buffer::config& vulkan_depth_buffer::cfg() const
{
	return _cfg;
}

std::unique_ptr<vulkan_depth_buffer> vulkan_depth_buffer::make(const config& cfg)
{
	VkFormat vulkan_format = to_vulkan_format(cfg.format);
	
	if (VK_FORMAT_UNDEFINED == vulkan_format)
	{
		return nullptr;
	}

	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties(cfg.physical_device, vulkan_format, &props);

	if (0 == (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
	{
		LOG_CRITICAL("%s not supported as a depth attachment format", to_string(cfg.format).data());
		return false;
	}

	VkImageCreateInfo image_info = {};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent = { (uint32_t)cfg.res.x, (uint32_t)cfg.res.y , 1 };
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.format = vulkan_format;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	image_info.samples = cfg.sample_count;
	image_info.sharingMode = cfg.sharing_mode;


	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = vulkan_format;
	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;

	
	VkSamplerCreateInfo sampler_info = {};
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.magFilter = VK_FILTER_LINEAR;
	sampler_info.minFilter = VK_FILTER_LINEAR;
	sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.anisotropyEnable = VK_TRUE;
	sampler_info.maxAnisotropy = 16;
	sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
	sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_info.minLod = 0;
	sampler_info.maxLod = (float)image_info.mipLevels;
	sampler_info.mipLodBias = 0;


	vulkan_image::config image_cfg = {};
	image_cfg.physical_device = cfg.physical_device;
	image_cfg.device = cfg.device;
	image_cfg.memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	image_cfg.image_info = image_info;
	image_cfg.view_info = view_info;
	image_cfg.sampler_info = sampler_info;


	if (vulkan_image::validate(image_cfg))
	{
		return std::unique_ptr < vulkan_depth_buffer >(
			new vulkan_depth_buffer(
				cfg,
				image_cfg));
	}

	return nullptr;
}

vulkan_depth_buffer::vulkan_depth_buffer(
	const config& cfg,
	const vulkan_image::config& image_cfg)
	: vulkan_image_target_t(image_cfg)
	, _cfg(cfg)
{
}
