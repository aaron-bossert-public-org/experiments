
#pragma once

#include "vulkan/texture/vulkan_image.h"
#include "vulkan/texture/vulkan_sampler.h"

namespace igpu
{
	template < typename T >
	class vulkan_image_target_t : public T
	{
	public:
		using config = typename T::config;

		const config& cfg() const override
		{
			return _cfg;
		}

		vulkan_image& gpu_object() override
		{
			return _gpu_image;
		}

		const vulkan_image& gpu_object() const override
		{
			return _gpu_image;
		}

		static std::unique_ptr< vulkan_image_target_t > make(
			const config& cfg,
			const vulkan_image::config& image_cfg )
		{
			if ( vulkan_image::validate( image_cfg ) )
			{
				return std::unique_ptr< vulkan_image_target_t >(
					new vulkan_image_target_t( cfg, image_cfg ) );
			}

			return nullptr;
		}

	protected:
		vulkan_image_target_t(
			const config& cfg,
			const vulkan_image::config& image_cfg )
			: _cfg( cfg )
			, _gpu_image( image_cfg )
		{}

	private:
		const config _cfg;
		vulkan_image _gpu_image;
	};

	template < typename CONFIG >
	vulkan_image::config to_vulkan_image_info(
		const CONFIG& cfg,
		const glm::ivec2& res,
		VkFormat format,
		VkImageUsageFlags usage,
		VkSampleCountFlagBits samples,
		VkSharingMode sharing_mode,
		VkImageAspectFlags aspect_mask,
		size_t mipmap_count )
	{
		vulkan_image::config image_cfg = {};
		image_cfg.physical_device = cfg.vk.physical_device;
		image_cfg.device = cfg.vk.device;
		image_cfg.memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		VkImageCreateInfo& image_info = image_cfg.image_info;
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent = {
			(uint32_t)res.x,
			(uint32_t)res.y,
			1,
		};
		image_info.mipLevels = (uint32_t)mipmap_count;
		image_info.arrayLayers = 1;
		image_info.format = format;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.usage = usage;
		image_info.samples = samples;
		image_info.sharingMode = sharing_mode;


		VkImageViewCreateInfo& view_info = image_cfg.view_info;
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = format;
		view_info.subresourceRange.aspectMask = aspect_mask;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = (uint32_t)mipmap_count;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;


		VkSamplerCreateInfo& sampler_info = image_cfg.sampler_info;
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.magFilter = to_vulkan_filter( cfg.sampler.mag_filter );
		sampler_info.minFilter = to_vulkan_filter( cfg.sampler.min_filter );
		sampler_info.addressModeU = to_vulkan_address( cfg.sampler.addressu );
		sampler_info.addressModeV = to_vulkan_address( cfg.sampler.addressv );
		sampler_info.addressModeW =
			to_vulkan_address( sampler::address::DEFAULT );
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

		return image_cfg;
	}
}
