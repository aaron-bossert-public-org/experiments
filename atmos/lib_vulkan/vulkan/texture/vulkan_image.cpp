
#include "vulkan/texture/vulkan_image.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/context/vulkan_abandon_manager.h"
#include "vulkan/shader/vulkan_parameter.h"
#include "vulkan/sync/vulkan_barrier_manager.h"
#include "vulkan/sync/vulkan_synchronization.h"

#include <algorithm>

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

	void mip_transfer_barrier(
		VkCommandBuffer command_buffer,
		VkImage image,
		uint32_t mip,
		igpu::decorator decorator )
	{
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseMipLevel = mip;

		if ( decorator == decorator::READABLE )
		{
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		}
		else if ( decorator == decorator::WRITABLE )
		{
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		else
		{
			LOG_CRITICAL(
				"unhandled decorator %s",
				to_string( decorator ).data() );
			return;
		}

		vkCmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			(VkDependencyFlags)0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier );
	}

	std::string perf_name( VkImageUsageFlags usage )
	{
		std::string result;
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

vulkan_image::vulkan_image( const config& cfg )
	: _gpu_mem_metric(
		  perf::category::GPU_MEM_USAGE,
		  perf_name( cfg.image_info.usage ) )
{
	_cfg.image_info.usage = cfg.image_info.usage;
	reset( &cfg );
}

vulkan_image::vulkan_image( VkImageUsageFlags usage )
	: _gpu_mem_metric( perf::category::GPU_MEM_USAGE, perf_name( usage ) )
{
	_cfg.image_info.usage = usage;
}

vulkan_image::~vulkan_image()
{
	vulkan_resource::wait_pending_jobs();
	reset();
}

const vulkan_image::config& vulkan_image::cfg() const
{
	return _cfg;
}

VkImageView vulkan_image::vk_image_view() const
{
	return _allocation.image_view;
}

VkSampler vulkan_image::vk_sampler() const
{
	return _allocation.sampler;
}

void vulkan_image::reset( const config* p_cfg )
{
	if ( !p_cfg || p_cfg->memory == memory_type::WRITE_COMBINED )
	{
		if ( _allocation.image )
		{
			auto& abandon_manager = _cfg.synchronization->abandon_manager();

			abandon_manager.abandon( _allocation.sampler );
			abandon_manager.abandon( _allocation.image_view );
			abandon_manager.abandon( _allocation.device_memory );
			abandon_manager.abandon( _allocation.image );
		}
		else
		{}

		if ( !p_cfg )
		{
			auto usage = _cfg.image_info.usage;
			_cfg = {};
			_cfg.image_info.usage = usage;

			_allocation = {};
			_gpu_mem_metric.reset();
		}
		else if ( _cfg.image_info.usage != p_cfg->image_info.usage )
		{
			LOG_CRITICAL( "cannot modify image usage after it is created" );
		}
		else
		{
			_cfg = *p_cfg;

			_allocation.image = create_image( _cfg );
			auto alloc_info = create_alloc_info( _cfg, _allocation.image );

			_allocation.device_memory =
				create_device_memory( _cfg, alloc_info, _allocation.image );

			_allocation.image_view =
				create_image_view( _cfg, _allocation.image );

			_allocation.sampler = create_sampler( _cfg );

			_gpu_mem_metric.add( alloc_info.allocationSize );
		}

		vulkan_resource::reinitialized(
			nullptr,
			VK_IMAGE_LAYOUT_UNDEFINED,
			{ decorator::NOTHING, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT } );
	}
	else
	{
		LOG_CRITICAL(
			"not implemented for type %s",
			to_string( p_cfg->memory ).data() );
	}
}

void vulkan_image::copy_from(
	vulkan_barrier_manager& barrier_manager,
	vulkan_buffer& buffer )
{
	if ( _cfg.memory == memory_type::WRITE_COMBINED )
	{
		barrier_manager.submit_frame_job(
			_cfg.synchronization->cfg().transfer_queue,
			{
				frame_job_barrier(
					&buffer,
					decorator::READABLE,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_ACCESS_TRANSFER_READ_BIT ),

				frame_job_barrier(
					this,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					decorator::WRITABLE,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_ACCESS_TRANSFER_WRITE_BIT ),
			},
			[&]( VkCommandBuffer command_buffer ) {
				VkBufferImageCopy region = {};
				region.bufferOffset = 0;
				region.bufferRowLength = 0;
				region.bufferImageHeight = 0;
				region.imageSubresource.aspectMask =
					_cfg.view_info.subresourceRange.aspectMask;
				region.imageSubresource.mipLevel = 0;
				region.imageSubresource.baseArrayLayer = 0;
				region.imageSubresource.layerCount = 1;
				region.imageOffset = {};
				region.imageExtent = _cfg.image_info.extent;


				vkCmdCopyBufferToImage(
					command_buffer,
					buffer.vk_buffer(),
					_allocation.image,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1,
					&region );
			} );
	}
	else
	{
		LOG_CRITICAL(
			"not implemented for type %s",
			to_string( _cfg.memory ).data() );
	}
}

void vulkan_image::generate_mipmaps( vulkan_barrier_manager& barrier_manager )
{
	if ( !can_generate_mipmaps(
			 _cfg.physical_device,
			 _cfg.image_info.format,
			 _cfg.image_info.tiling ) )
	{
		LOG_CRITICAL(
			"cannot generate mipmaos because vk format %d does not support "
			"blitting for tiling mode %d",
			(int)_cfg.image_info.format,
			(int)_cfg.image_info.tiling );
	}
	else
	{
		barrier_manager.submit_frame_job(

			_cfg.synchronization->cfg().graphics_queue,
			{
				frame_job_barrier(
					this,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					decorator::WRITABLE,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_ACCESS_TRANSFER_WRITE_BIT ),
			},
			[&]( VkCommandBuffer command_buffer ) {
				int32_t mip_width = _cfg.image_info.extent.width;
				int32_t mip_height = _cfg.image_info.extent.height;

				uint32_t mip_count = _cfg.view_info.subresourceRange.levelCount;

				for ( uint32_t i = 1; i < mip_count; ++i )
				{
					mip_transfer_barrier(
						command_buffer,
						_allocation.image,
						i - 1,
						decorator::READABLE );

					VkImageBlit blit = {};

					blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					blit.srcSubresource.mipLevel = i - 1;
					blit.srcSubresource.baseArrayLayer = 0;
					blit.srcSubresource.layerCount = 1;
					blit.srcOffsets[0] = {};
					blit.srcOffsets[1] = { mip_width, mip_height, 1 };

					mip_width = std::max( 1, mip_width / 2 );
					mip_height = std::max( 1, mip_height / 2 );

					blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					blit.dstSubresource.mipLevel = i;
					blit.dstSubresource.baseArrayLayer = 0;
					blit.dstSubresource.layerCount = 1;
					blit.dstOffsets[0] = {};
					blit.dstOffsets[1] = { mip_width, mip_height, 1 };

					vkCmdBlitImage(
						command_buffer,
						_allocation.image,
						VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						_allocation.image,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1,
						&blit,
						VK_FILTER_LINEAR );

					mip_transfer_barrier(
						command_buffer,
						_allocation.image,
						i - 1,
						decorator::WRITABLE );
				}
			} );
	}
}

bool vulkan_image::can_generate_mipmaps(
	VkPhysicalDevice physical_device,
	VkFormat format,
	VkImageTiling tiling )
{
	VkFlags required = VK_FORMAT_FEATURE_BLIT_SRC_BIT |
		VK_FORMAT_FEATURE_BLIT_DST_BIT |
		VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

	VkFlags actual = 0;
	VkFormatProperties format_properties;
	vkGetPhysicalDeviceFormatProperties(
		physical_device,
		format,
		&format_properties );

	switch ( tiling )
	{
	case VK_IMAGE_TILING_OPTIMAL:
		actual = format_properties.optimalTilingFeatures;
		break;
	case VK_IMAGE_TILING_LINEAR:
		actual = format_properties.linearTilingFeatures;
		break;
	}

	return ( actual & required ) == required;
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

bool vulkan_image::is_valid_layout( VkImageLayout layout ) const
{
	return layout != VK_IMAGE_LAYOUT_MAX_ENUM;
}

vulkan_resource::state& vulkan_image::resource_state()
{
	return _resource_state;
}

const vulkan_resource::state& vulkan_image::resource_state() const
{
	return _resource_state;
}

void vulkan_image::update_descriptor_set(
	VkDescriptorSet descriptor_set,
	const vulkan_parameter::config& parameter_config,
	size_t array_element ) const
{
	VkDescriptorImageInfo image_descriptor{
		_allocation.sampler,
		_allocation.image_view,
		parameter_config.vk.image_layout,
	};

	VkWriteDescriptorSet write_descriptor = {
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		descriptor_set,
	};

	write_descriptor.dstBinding = (uint32_t)parameter_config.binding;
	write_descriptor.dstArrayElement = (uint32_t)array_element;
	write_descriptor.descriptorCount = (uint32_t)parameter_config.array_size;
	write_descriptor.descriptorType = parameter_config.vk.descriptor_type;
	write_descriptor.pImageInfo = &image_descriptor;

	vkUpdateDescriptorSets( _cfg.device, 1, &write_descriptor, 0, nullptr );
}

void vulkan_image::push_barrier(
	uint32_t target_queue_family_index,
	vulkan_barrier_manager* barrier_manager,
	uint32_t src_queue_family_index,
	uint32_t dst_queue_family_index,
	VkImageLayout src_layout,
	VkImageLayout dst_layout,
	const vulkan_job_scope& src_scope,
	const vulkan_job_scope& dst_scope ) const
{
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcAccessMask = (VkFlags)src_scope.access;
	barrier.dstAccessMask = (VkFlags)dst_scope.access;
	barrier.oldLayout = src_layout;
	barrier.newLayout = dst_layout;
	barrier.srcQueueFamilyIndex = src_queue_family_index;
	barrier.dstQueueFamilyIndex = dst_queue_family_index;
	barrier.image = _allocation.image;
	barrier.subresourceRange = _cfg.view_info.subresourceRange;

	barrier_manager->push_barrier(
		target_queue_family_index,
		src_scope.stages,
		dst_scope.stages,
		barrier );
}

std::unique_ptr< vulkan_image > vulkan_image::make( const config& cfg )
{
	return std::unique_ptr< vulkan_image >( new vulkan_image( cfg ) );
}