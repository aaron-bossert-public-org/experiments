
#include "vulkan/texture/vulkan_image.h"

using namespace igpu;

namespace
{
	VkImage create_image( const vulkan_image::config& cfg )
	{
		VkImage image = nullptr;
		vkCreateImage( cfg.device, &cfg.image_info, nullptr, &image );
		return image;
	}

	VkImageView create_image_view(
		const vulkan_image::config& cfg,
		VkImage image )
	{
		VkImageViewCreateInfo view_info = cfg.view_info;
		view_info.image = image;

		VkImageView image_view = nullptr;
		vkCreateImageView( cfg.device, &view_info, nullptr, &image_view );
		return image_view;
	}

	bool find_memory_type(
		const vulkan_image::config& cfg,
		uint32_t type_filter,
		uint32_t* out_memory_type )
	{
		VkFlags property_flags = (VkFlags)cfg.memory_properties;
		VkPhysicalDeviceMemoryProperties mem_properties;
		vkGetPhysicalDeviceMemoryProperties(
			cfg.physical_device,
			&mem_properties );

		for ( uint32_t i = 0; i < mem_properties.memoryTypeCount; i++ )
		{
			if ( ( type_filter & ( 1 << i ) ) &&
				 ( mem_properties.memoryTypes[i].propertyFlags &
				   property_flags ) == property_flags )
			{
				*out_memory_type = i;
				return true;
			}
		}

		return false;
	}

	VkMemoryAllocateInfo create_alloc_info(
		const vulkan_image::config& cfg,
		VkImage image )
	{
		VkMemoryRequirements mem_requirements;
		vkGetImageMemoryRequirements( cfg.device, image, &mem_requirements );

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirements.size;


		if ( !find_memory_type(
				 cfg,
				 mem_requirements.memoryTypeBits,
				 &alloc_info.memoryTypeIndex ) )
		{
			LOG_CRITICAL( "failed to find suitable memory type!" );
			return {};
		}

		return alloc_info;
	}

	VkDeviceMemory create_device_memory(
		const vulkan_image::config& cfg,
		const VkMemoryAllocateInfo& alloc_info,
		VkImage image )
	{
		VkDeviceMemory device_memory = nullptr;
		vkAllocateMemory( cfg.device, &alloc_info, nullptr, &device_memory );
		vkBindImageMemory( cfg.device, image, device_memory, 0 );
		return device_memory;
	}
	VkSampler create_sampler( const vulkan_image::config& cfg )
	{
		VkSampler sampler = nullptr;
		vkCreateSampler( cfg.device, &cfg.sampler_info, nullptr, &sampler );
		return sampler;
	}

	std::string perf_name( const vulkan_image::config& cfg )
	{
		std::string result;
		auto usage = cfg.image_info.usage;
		if ( VK_IMAGE_USAGE_TRANSFER_SRC_BIT & usage )
		{
			result += "transfer src/";
		}
		if ( VK_IMAGE_USAGE_TRANSFER_DST_BIT & usage )
		{
			result += "transfer dst/";
		}
		if ( VK_IMAGE_USAGE_SAMPLED_BIT & usage )
		{
			result += "sampled/";
		}
		if ( VK_IMAGE_USAGE_STORAGE_BIT & usage )
		{
			result += "storage/";
		}
		if ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT & usage )
		{
			result += "color attachment/";
		}
		if ( VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT & usage )
		{
			result += "depth stencil attachment/";
		}
		if ( VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT & usage )
		{
			result += "transient/";
		}
		if ( VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT & usage )
		{
			result += "input attachment/";
		}
		if ( VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV & usage )
		{
			result += "shading rate image/";
		}
		if ( VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT & usage )
		{
			result += "fragment density map/";
		}

		result.resize( result.size() - 1 );
		return result;
	}
}

bool vulkan_image::validate( const config& cfg )
{
	if ( !cfg.physical_device )
	{
		LOG_CRITICAL( "physical device is null" );
	}
	else if ( !cfg.device )
	{
		LOG_CRITICAL( "device is null" );
	}
	else if ( cfg.memory_properties == 0 )
	{
		LOG_CRITICAL( "no memory properties specified is null" );
	}
	else if ( cfg.image_info.sType == 0 )
	{
		LOG_CRITICAL( "info.sType is zero" );
	}
	else if (
		cfg.image_info.extent.width == 0 || cfg.image_info.extent.height == 0 ||
		cfg.image_info.extent.depth == 0 )
	{
		LOG_CRITICAL(
			"info.extent(%d, %d, %d) has a zero coordinate",
			cfg.image_info.extent.width,
			cfg.image_info.extent.height,
			cfg.image_info.extent.depth );
	}
	else if ( cfg.image_info.mipLevels == 0 )
	{
		LOG_CRITICAL( "info.mipLevels is zero" );
	}
	else if ( cfg.image_info.arrayLayers == 0 )
	{
		LOG_CRITICAL( "info.arrayLayers is zero" );
	}
	else if ( cfg.image_info.format == VK_FORMAT_UNDEFINED )
	{
		LOG_CRITICAL( "info.format is VK_FORMAT_UNDEFINED" );
	}
	else if ( cfg.image_info.usage == 0 )
	{
		LOG_CRITICAL( "info.usage is zero" );
	}
	else if ( cfg.image_info.samples == 0 )
	{
		LOG_CRITICAL( "info.samples is zero" );
	}
	else if ( cfg.view_info.sType == 0 )
	{
		LOG_CRITICAL( "view_info.sType is zero" );
	}
	else if ( cfg.view_info.format == VK_FORMAT_UNDEFINED )
	{
		LOG_CRITICAL( "view_info.format is VK_FORMAT_UNDEFINED" );
	}
	else if ( cfg.view_info.subresourceRange.aspectMask == 0 )
	{
		LOG_CRITICAL( "view_info.subresourceRange.aspectMask is zero" );
	}
	else if ( cfg.view_info.subresourceRange.levelCount == 0 )
	{
		LOG_CRITICAL( "view_info.subresourceRange.levelCount is zero" );
	}
	else if ( cfg.view_info.subresourceRange.layerCount == 0 )
	{
		LOG_CRITICAL( "view_info.subresourceRange.layerCount is zero" );
	}
	else
	{
		return true;
	}

	return false;
}

vulkan_image::vulkan_image( const config& cfg )
	: _cfg( cfg )
	, _image( create_image( cfg ) )
	, _alloc_info( create_alloc_info( cfg, _image ) )
	, _device_memory( create_device_memory( cfg, _alloc_info, _image ) )
	, _image_view( create_image_view( cfg, _image ) )
	, _sampler( create_sampler( cfg ) )
	, _gpu_mem_metric( perf::category::GPU_MEM_USAGE, perf_name( cfg ) )
{
	_gpu_mem_metric.add( _alloc_info.allocationSize ); // should not hit
}

const vulkan_image::config& vulkan_image::cfg() const
{
	return _cfg;
}

const vulkan_image::ownership& vulkan_image::owner() const
{
	return _owner;
}

void vulkan_image::owner( const ownership& owner )
{
	_owner = owner;
}

VkImage vulkan_image::get() const
{
	return _image;
}

VkDeviceMemory vulkan_image::device_memory() const
{
	return _device_memory;
}

VkImageView vulkan_image::image_view() const
{
	return _image_view;
}

VkSampler vulkan_image::sampler() const
{
	return _sampler;
}

vulkan_gpu_object::state& vulkan_image::object_state()
{
	return _object_state;
}

vulkan_image::~vulkan_image()
{
	vulkan_gpu_object::wait_on_fences();

	if ( _sampler )
	{
		vkDestroySampler( _cfg.device, _sampler, nullptr );
	}

	if ( _image_view )
	{
		vkDestroyImageView( _cfg.device, _image_view, nullptr );
	}

	if ( _device_memory )
	{
		vkFreeMemory( _cfg.device, _device_memory, nullptr );
	}

	if ( _image )
	{
		vkDestroyImage( _cfg.device, _image, nullptr );
	}
}

std::unique_ptr< vulkan_image > vulkan_image::make( const config& cfg )
{
	return std::unique_ptr< vulkan_image >( new vulkan_image( cfg ) );
}
