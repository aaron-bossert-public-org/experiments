
#include "vulkan/sync/vulkan_synchronization.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_fence.h"
#include "vulkan/sync/vulkan_queue.h"
#include "vulkan/texture/vulkan_image.h"


using namespace igpu;

namespace
{
	struct submit_context
	{
		VkSubmitInfo info;

		operator const VkSubmitInfo*() const
		{
			return &info;
		}
		operator VkCommandBuffer() const
		{
			return *info.pCommandBuffers;
		}
	};

	// allocate/build/execute/free a temporary command buffer before function
	// exits
	template < typename SubmissonBuilder, typename... Args >
	void submit_sync( vulkan_queue& queue, SubmissonBuilder&& builder )
	{
		vulkan_command_buffer command_buffer( {
			queue.cfg().device,
			queue.command_pool(),
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		} );
		VkCommandBuffer raw_command_buffer = command_buffer.get();


		submit_context submit_context = {};
		submit_context.info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_context.info.commandBufferCount = 1;
		submit_context.info.pCommandBuffers = &raw_command_buffer;

		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer( raw_command_buffer, &begin_info );
		builder( raw_command_buffer );
		vkEndCommandBuffer( raw_command_buffer );

		vkQueueSubmit( queue.get(), 1, &submit_context.info, VK_NULL_HANDLE );
		vkQueueWaitIdle( queue.get() );
	}

	// allocate/build/submit command buffer to be executed asynnchronously
	template < typename SubmissonBuilder >
	scoped_ptr< vulkan_fence > submit(
		vulkan_queue& queue,
		SubmissonBuilder&& builder )
	{
		queue.free_completed_commands();

		vulkan_command_buffer::config cfg = {
			queue.cfg().device,
			queue.command_pool(),
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		};

		queue.pending_commands().emplace_back(
			cfg,
			vulkan_fence::make( {
				queue.cfg().device,
				queue.get(),
				queue.get_increment_submit_index(),
			} ) );
		vulkan_command_buffer& command_buffer = queue.pending_commands().back();
		VkCommandBuffer raw_command_buffer = command_buffer.get();


		submit_context submit_context = {};
		submit_context.info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_context.info.commandBufferCount = 1;
		submit_context.info.pCommandBuffers = &raw_command_buffer;

		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer( raw_command_buffer, &begin_info );
		builder( raw_command_buffer );
		vkEndCommandBuffer( raw_command_buffer );

		vkQueueSubmit(
			queue.get(),
			1,
			&submit_context.info,
			command_buffer.fence()->get() );

		return command_buffer.fence();
	}

	// allocate/build/execute/free a temporary command buffer before function
	// exits ensure memory/execution dependencies are added for args
	template < typename CommandBuilder, typename... Args >
	void submit_sync(
		const scoped_ptr< vulkan_queue >& queue,
		CommandBuilder&& command_builder,
		Args... args )
	{
		submit_sync( *queue, [&]( VkCommandBuffer command_buffer ) {
			command_builder(
				command_buffer,
				add_dependency( command_buffer, queue, args )... );
		} );
	}

	// allocate/build/submit command buffer to be executed asynnchronously
	// ensure memory/execution dependencies are added for args
	template < typename CommandBuilder, typename... Args >
	scoped_ptr< vulkan_fence > submit(
		const scoped_ptr< vulkan_queue >& queue,
		CommandBuilder&& command_builder,
		Args... args )
	{
		return submit( *queue, [&]( VkCommandBuffer command_buffer ) {
			command_builder(
				command_buffer,
				add_dependency( command_buffer, queue, args )... );
		} );
	}

	struct image_dependency
	{
		vulkan_image& image;
		VkImageLayout layout;
		VkAccessFlags access;
		VkPipelineStageFlags stage;
		VkDependencyFlags dependency;
	};

	image_dependency dependency(
		vulkan_image& image,
		VkImageLayout layout,
		VkAccessFlags access,
		VkPipelineStageFlags stage,
		VkDependencyFlags dependency = (VkDependencyFlags)0 )
	{
		return {
			image,
			layout,
			access,
			stage,
			dependency,
		};
	}

	VkImage add_dependency(
		VkCommandBuffer command_buffer,
		const scoped_ptr< vulkan_queue >& queue,
		const image_dependency& image_dependency )
	{
		vulkan_image& image = image_dependency.image;

		const vulkan_image::ownership& current_owner = image.owner();
		vulkan_image::ownership target_owner = {
			image_dependency.layout,
			image_dependency.access,
			image_dependency.stage,
			image_dependency.dependency,
			queue,
		};

		// apparently this is slower than VkMemoryBarrier, need an api to handle
		// these in bulk with VkMemoryBarrier.
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.srcAccessMask = (VkFlags)current_owner.access;
		barrier.dstAccessMask = (VkFlags)target_owner.access;
		barrier.oldLayout = current_owner.layout;
		barrier.newLayout = target_owner.layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image.get();
		barrier.subresourceRange = image.cfg().view_info.subresourceRange;

		// cannot add dependency with null queue
		if ( !target_owner.queue )
		{
			LOG_CRITICAL( "target owner queue is null" );
		}
		// just a layout transition to initialize image to the specified state,
		// no dependency actually added via vkbarriers this branch
		else if ( !current_owner.queue )
		{
			vkCmdPipelineBarrier(
				command_buffer,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				target_owner.stage,
				target_owner.dependency,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&barrier );
		}
		// add dependency to some operation within the queue that has already
		// aquired this buffer
		else if ( current_owner.queue == target_owner.queue )
		{
			if ( target_owner.access != current_owner.access ||
				 target_owner.stage != current_owner.stage )
			{
				vkCmdPipelineBarrier(
					command_buffer,
					current_owner.stage,
					target_owner.stage,
					target_owner.dependency,
					0,
					nullptr,
					0,
					nullptr,
					1,
					&barrier );
			}
		}

		// release current queue's ownership of this buffer and aquire ownership
		// on target_queue
		else if ( current_owner.queue )
		{
			VkImageMemoryBarrier source_barrier = barrier;
			source_barrier.dstAccessMask = 0;
			source_barrier.srcQueueFamilyIndex =
				current_owner.queue->cfg().family_index;
			source_barrier.dstQueueFamilyIndex =
				target_owner.queue->cfg().family_index;

			VkImageMemoryBarrier dest_barrier = barrier;
			dest_barrier.srcAccessMask = 0;
			dest_barrier.srcQueueFamilyIndex =
				current_owner.queue->cfg().family_index;
			dest_barrier.dstQueueFamilyIndex =
				target_owner.queue->cfg().family_index;

			// Relese exclusive ownersip on _queue
			submit_sync(
				current_owner.queue,
				[&]( VkCommandBuffer current_owner_command_buffer ) {
					vkCmdPipelineBarrier(
						current_owner_command_buffer,
						current_owner.stage,
						VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&source_barrier );
				} );

			// Aquire exclusive ownership on target_queue

			vkCmdPipelineBarrier(
				command_buffer,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				target_owner.stage,
				target_owner.dependency,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&dest_barrier );
		}

		image.owner( target_owner );

		return image.get();
	}

	struct buffer_dependency
	{
		vulkan_buffer& buffer;
		VkAccessFlags access;
		VkPipelineStageFlags stage;
		VkDependencyFlags dependency;
	};

	buffer_dependency dependency(
		vulkan_buffer& buffer,
		VkAccessFlags access,
		VkPipelineStageFlags stage,
		VkDependencyFlags dependency = (VkDependencyFlagBits)0 )
	{
		return {
			buffer,
			access,
			stage,
			dependency,
		};
	};

	VkBuffer add_dependency(
		VkCommandBuffer command_buffer,
		const scoped_ptr< vulkan_queue >& queue,
		const buffer_dependency& buffer_dependency )
	{
		vulkan_buffer& buffer = buffer_dependency.buffer;
		const vulkan_buffer::ownership& current_owner = buffer.owner();
		vulkan_buffer::ownership target_owner = {
			buffer_dependency.access,
			buffer_dependency.stage,
			buffer_dependency.dependency,
			queue,
		};

		// apparently this is slower than VkMemoryBarrier, need an api to handle
		// these in bulk with VkMemoryBarrier.
		VkBufferMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.srcAccessMask = (VkFlags)current_owner.access;
		barrier.dstAccessMask = (VkFlags)target_owner.access;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.buffer = buffer.get();
		barrier.offset = 0;
		barrier.size = buffer.mapped_view().byte_size();

		// cannot add dependency with null queue
		if ( !target_owner.queue )
		{
			LOG_CRITICAL( "target owner queue is null" );
		}
		// add dependency to some operation within the queue that has already
		// aquired this buffer
		else if ( current_owner.queue == target_owner.queue )
		{
			if ( target_owner.access != current_owner.access ||
				 target_owner.stage != current_owner.stage )
			{
				vkCmdPipelineBarrier(
					command_buffer,
					current_owner.stage,
					target_owner.stage,
					target_owner.dependency,
					0,
					nullptr,
					1,
					&barrier,
					0,
					nullptr );
			}
		}

		// release current queue's ownership of this buffer and aquire ownership
		// on target_queue
		else if ( current_owner.queue )
		{
			VkBufferMemoryBarrier source_barrier = barrier;
			source_barrier.dstAccessMask = 0;
			source_barrier.srcQueueFamilyIndex =
				current_owner.queue->cfg().family_index;
			source_barrier.dstQueueFamilyIndex =
				target_owner.queue->cfg().family_index;

			VkBufferMemoryBarrier dest_barrier = barrier;
			dest_barrier.srcAccessMask = 0;
			dest_barrier.srcQueueFamilyIndex =
				current_owner.queue->cfg().family_index;
			dest_barrier.dstQueueFamilyIndex =
				target_owner.queue->cfg().family_index;

			// Relese exclusive ownersip on _queue
			submit_sync(
				current_owner.queue,
				[&]( VkCommandBuffer current_owner_command_buffer ) {
					vkCmdPipelineBarrier(
						current_owner_command_buffer,
						current_owner.stage,
						VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
						0,
						0,
						nullptr,
						1,
						&source_barrier,
						0,
						nullptr );
				} );

			// Aquire exclusive ownership on target_queue

			vkCmdPipelineBarrier(
				command_buffer,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				target_owner.stage,
				target_owner.dependency,
				0,
				nullptr,
				1,
				&dest_barrier,
				0,
				nullptr );
		}

		buffer.owner( target_owner );

		return buffer.get();
	}
}

const vulkan_synchronization::config& vulkan_synchronization::cfg() const
{
	return _cfg;
}

VmaAllocator vulkan_synchronization::vma()
{
	return _vma;
}

void vulkan_synchronization::copy(
	vulkan_buffer& src,
	vulkan_buffer& dst,
	uint32_t size,
	uint32_t src_offset,
	uint32_t dst_offset )
{
	submit_sync(
		_cfg.transfer_queue,
		[&]( VkCommandBuffer command_buffer,
			 VkBuffer src_buffer,
			 VkBuffer dst_buffer ) {
			VkBufferCopy region = {};
			region.srcOffset = src_offset;
			region.dstOffset = dst_offset;
			region.size = size;

			vkCmdCopyBuffer(
				command_buffer,
				src_buffer,
				dst_buffer,
				1,
				&region );
		},
		dependency(
			src,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT ),
		dependency(
			dst,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT ) );
}

void vulkan_synchronization::copy(
	vulkan_buffer& src,
	vulkan_image& dst,
	uint32_t src_offset )
{
	submit_sync(
		_cfg.transfer_queue,
		[&]( VkCommandBuffer command_buffer,
			 VkBuffer src_buffer,
			 VkImage dst_image ) {
			VkBufferImageCopy region = {};
			region.bufferOffset = src_offset;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask =
				dst.cfg().view_info.subresourceRange.aspectMask;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageOffset = {};
			region.imageExtent = dst.cfg().image_info.extent;

			vkCmdCopyBufferToImage(
				command_buffer,
				src_buffer,
				dst_image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&region );
		},
		dependency(
			src,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT ),
		dependency(
			dst,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT ) );
}


bool vulkan_synchronization::can_generate_mipmaps(
	VkFormat format,
	VkImageTiling tiling )
{
	VkFlags required = VK_FORMAT_FEATURE_BLIT_SRC_BIT |
		VK_FORMAT_FEATURE_BLIT_DST_BIT |
		VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;


	VkFlags actual = 0;
	VkFormatProperties format_properties;
	vkGetPhysicalDeviceFormatProperties(
		_cfg.physical_device,
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
void vulkan_synchronization::generate_mipmaps( vulkan_image& image )
{
	if ( false ==
		 can_generate_mipmaps(
			 image.cfg().image_info.format,
			 image.cfg().image_info.tiling ) )
	{
		LOG_CRITICAL(
			"vk format %d does not support linear blitting!",
			image.cfg().image_info.format );
	}

	submit_sync(
		_cfg.graphics_queue,
		[&]( VkCommandBuffer command_buffer, VkImage vk_image ) {
			VkImageMemoryBarrier barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = vk_image;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.subresourceRange.levelCount = 1;

			int32_t mip_width = image.cfg().image_info.extent.width;
			int32_t mip_height = image.cfg().image_info.extent.height;

			// setting by hand, currently there isn't a way to represent per-mip
			// ownership so we do the barriers by hand and track ownership
			// manually. In the future support should be added to track
			// ownership on the mip level
			image.owner( {
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VK_ACCESS_TRANSFER_READ_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				_cfg.graphics_queue,
			} );

			for ( uint32_t i = 1;
				  i < image.cfg().view_info.subresourceRange.levelCount;
				  i++ )
			{
				barrier.subresourceRange.baseMipLevel = i;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				vkCmdPipelineBarrier(
					command_buffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					0,
					0,
					nullptr,
					0,
					nullptr,
					1,
					&barrier );

				VkImageBlit blit = {};
				blit.srcOffsets[0] = {};
				blit.srcOffsets[1] = {
					mip_width,
					mip_height,
					1,
				};
				blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.srcSubresource.mipLevel = i - 1;
				blit.srcSubresource.baseArrayLayer = 0;
				blit.srcSubresource.layerCount = 1;
				blit.dstOffsets[0] = {};
				blit.dstOffsets[1] = {
					mip_width > 1 ? mip_width / 2 : 1,
					mip_height > 1 ? mip_height / 2 : 1,
					1,
				};
				blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.dstSubresource.mipLevel = i;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount = 1;

				vkCmdBlitImage(
					command_buffer,
					vk_image,
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					vk_image,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1,
					&blit,
					VK_FILTER_LINEAR );

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				vkCmdPipelineBarrier(
					command_buffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					0,
					0,
					nullptr,
					0,
					nullptr,
					1,
					&barrier );

				if ( mip_width > 1 )
					mip_width /= 2;
				if ( mip_height > 1 )
					mip_height /= 2;
			}
		},
		dependency(
			image,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT ) );
}

vulkan_synchronization::~vulkan_synchronization()
{
	vmaDestroyAllocator( _vma );
}

std::unique_ptr< vulkan_synchronization > vulkan_synchronization::make(
	const config& cfg )
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = cfg.physical_device;
	allocatorInfo.device = cfg.device;

	VmaAllocator vma;
	vmaCreateAllocator( &allocatorInfo, &vma );

	return std::unique_ptr< vulkan_synchronization >(
		new vulkan_synchronization( cfg, vma ) );
}

vulkan_synchronization::vulkan_synchronization(
	const config& cfg,
	VmaAllocator vma )
	: _cfg( cfg )
	, _vma( vma )
{}

#include "vulkan/context/vulkan_context.h"
#include "vulkan/defines/vulkan_includes.h"

#include "igpu/window/window.h"

#define GLM_ENABLE_EXPERIMENTAL 1
#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"


#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#pragma warning( push )
#pragma warning( disable : 4505 )

#include "vulkan/buffer/Vulkan_geometry.h"
#include "vulkan/buffer/vulkan_compute_buffer.h"
#include "vulkan/buffer/vulkan_index_buffer.h"
#include "vulkan/buffer/vulkan_vertex_buffer.h"
#include "vulkan/shader/vulkan_attribute_sequencer.h"
#include "vulkan/shader/vulkan_fragment_shader.h"
#include "vulkan/shader/vulkan_primitive_sequencer.h"
#include "vulkan/shader/vulkan_program.h"
#include "vulkan/shader/vulkan_render_states.h"
#include "vulkan/shader/vulkan_vertex_shader.h"
#include "vulkan/texture/vulkan_texture2d.h"
#include "vulkan/window/vulkan_back_buffer.h"

#include <array>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <vector>

const std::string MODEL_PATH = "cooked_assets/models/chalet.obj";
const std::string TEXTURE_PATH = "cooked_assets/textures/chalet.jpg";
const std::string VERTEX_PATH = "cooked_assets/shaders/shader.vert.spv";
const std::string FRAGMENT_PATH = "cooked_assets/shaders/shader.frag.spv";

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 col;
	glm::vec2 uv0;

	bool operator==( const Vertex& other ) const
	{
		return pos == other.pos && col == other.col && uv0 == other.uv0;
	}
};

namespace std
{
	template <>
	struct hash< Vertex >
	{
		size_t operator()( Vertex const& vertex ) const
		{
			return ( ( hash< glm::vec3 >()( vertex.pos ) ^
					   ( hash< glm::vec3 >()( vertex.col ) << 1 ) ) >>
					 1 ) ^
				( hash< glm::vec2 >()( vertex.uv0 ) << 1 );
		}
	};
}

struct UniformBufferBatch
{
	alignas( 16 ) glm::mat4 view;
	alignas( 16 ) glm::mat4 proj;
};


struct UniformBufferInstance
{
	alignas( 16 ) glm::mat4 model;
};

using namespace igpu;

class HelloTriangleApplication
{
private:
	scoped_ptr< vulkan_context > _context;
	const vulkan_window* _window = nullptr;

	VkInstance _instance = nullptr;
	VkPhysicalDevice _physical_device = nullptr;
	VkDevice _device = nullptr;

	scoped_ptr< vulkan_queue > _graphics_queue;
	scoped_ptr< vulkan_queue > _present_queue;

	VkPipeline _graphics_pipeline;

	struct new_texture_impl
	{
		HelloTriangleApplication* app;
		std::shared_ptr< texture2d > texture;
		VkSampler _texture_sampler;

		[[nodiscard]] bool create()
		{
			texture2d::config texture_cfg = {
				TEXTURE_PATH,
				sampler::filter::LINEAR,
				sampler::filter::LINEAR,
				sampler::address::WRAP,
				sampler::address::WRAP,
			};
			texture_cfg.can_auto_generate_mips = true;
			texture = app->_context->make_texture( texture_cfg );

			if ( false == app->load_buffer( TEXTURE_PATH, texture.get() ) )
			{
				return false;
			}

			// transition layout for use by graphics pipeline
			auto* vulkan = (vulkan_texture2d*)texture.get();
			submit_sync(
				app->_context->synchronization().cfg().graphics_queue,
				[&]( VkCommandBuffer, VkImage ) {},
				dependency(
					vulkan->gpu_object(),
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VK_ACCESS_SHADER_READ_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
						VK_PIPELINE_STAGE_VERTEX_SHADER_BIT ) );

			return true;
		}

		VkDescriptorImageInfo descriptor_info()
		{
			auto* vulkan = (vulkan_texture2d*)texture.get();
			vulkan_image& image = vulkan->gpu_object();

			VkDescriptorImageInfo info = {};
			info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			info.imageView = image.image_view();
			info.sampler = image.sampler();
			return info;
		}

		void destroy()
		{
			texture = nullptr;
		}
	};

	new_texture_impl texture_impl = {
		this,
	};

	struct new_geo_impl
	{
		[[nodiscard]] bool create()
		{
			index_buffer = app->_context->make_index_buffer( {
				index_format::UNSIGNED_INT,
			} );

			vertex_buffer = app->_context->make_vertex_buffer( {
				IGPU_VERT_CFG_OF( Vertex, pos, col, uv0 ),
			} );

			geometry = app->_context->make_geometry( {
				MODEL_PATH,
				topology::TRIANGLE_LIST,
				index_buffer,
				{
					vertex_buffer,
				},
			} );

			return (bool)geometry;
		}

		[[nodiscard]] bool upload(
			const std::vector< Vertex >& vertices,
			const std::vector< uint32_t >& indices )
		{
			buffer_view< char > vertices_view(
				sizeof( vertices[0] ) * vertices.size(),
				nullptr );

			buffer_view< char > indices_view(
				sizeof( indices[0] ) * indices.size(),
				nullptr );

			index_buffer->map( &indices_view );
			vertex_buffer->map( &vertices_view );

			if ( !indices_view.data() )
			{
				LOG_CRITICAL(
					"failed to obtain mapped memory from index buffer" );
				return false;
			}

			if ( !vertices_view.data() )
			{
				LOG_CRITICAL(
					"failed to obtain mapped memory from vertex buffer" );
				return false;
			}

			memcpy(
				indices_view.data(),
				indices.data(),
				indices_view.byte_size() );
			memcpy(
				vertices_view.data(),
				vertices.data(),
				vertices_view.byte_size() );

			index_buffer->unmap();
			vertex_buffer->unmap();

			return true;
		}

		void draw( VkCommandBuffer command_buffer )
		{
			auto* vulkan = ASSERT_CAST( vulkan_geometry*, geometry.get() );
			auto& vulkan_indices = vulkan->index_buffer();

			const int MAX_BUFFERS = attribute_indexer::MAX_PARAMETERS;
			ASSERT_CONTEXT( vulkan->cfg().vertex_buffers.size() < MAX_BUFFERS )
			VkBuffer vertex_buffers[MAX_BUFFERS];
			VkDeviceSize offsets[MAX_BUFFERS];

			for ( size_t i = 0; i < vulkan->cfg().vertex_buffers.size(); ++i )
			{
				offsets[i] = 0;
				vertex_buffers[i] =
					vulkan->vertex_buffer( i ).gpu_object().get();
			}

			vkCmdBindIndexBuffer(
				command_buffer,
				vulkan_indices.gpu_object().get(),
				0,
				vulkan_indices.cfg().vk.format );
			vkCmdBindVertexBuffers(
				command_buffer,
				0,
				1,
				vertex_buffers,
				offsets );
			vkCmdDrawIndexed(
				command_buffer,
				(uint32_t)vulkan->index_buffer().element_count(),
				1,
				0,
				0,
				0 );
		}

		void destroy()
		{
			index_buffer = nullptr;
			vertex_buffer = nullptr;
			geometry = nullptr;
		}

		HelloTriangleApplication* app;
		std::unique_ptr< geometry > geometry;
		std::shared_ptr< vertex_buffer > vertex_buffer;
		std::shared_ptr< index_buffer > index_buffer;
	};

	new_geo_impl geo_impl = {
		this,
	};

	struct batch_buffer_impl
	{
		HelloTriangleApplication* app;

		[[nodiscard]] bool create()
		{
			_compute_buffers.resize( app->_swap_image_count );

			for ( uint32_t i = 0; i < app->_swap_image_count; i++ )
			{
				_compute_buffers[i] = app->_context->make_compute_buffer( {} );

				if ( !_compute_buffers[i] )
				{
					return false;
				}
				update( i );
			}

			return true;
		}

		VkDescriptorBufferInfo descriptor_info( size_t swap_index )
		{
			auto vulkan =
				(vulkan_compute_buffer*)_compute_buffers[swap_index].get();
			VkBuffer buffer = vulkan->gpu_object().get();
			VkDescriptorBufferInfo buffer_info = {};
			buffer_info.buffer = buffer;
			buffer_info.offset = 0;
			buffer_info.range = sizeof( UniformBufferBatch );
			return buffer_info;
		}

		void update( uint32_t swap_index )
		{
			auto res = app->_context->back_buffer().cfg().res;

			auto view = buffer_view< UniformBufferBatch >( 1, nullptr );
			_compute_buffers[swap_index]->map( &view );
			UniformBufferBatch& ubo = view[0] = {};

#pragma warning( push )
#pragma warning( disable : 4127 ) // constant if condition
			ubo.view = glm::lookAt(
				glm::vec3( 2.0f, 2.0f, 2.0f ),
				glm::vec3( 0.0f, 0.0f, 0.0f ),
				glm::vec3( 0.0f, 0.0f, 1.0f ) );
			ubo.proj = glm::perspective(
				glm::radians( 45.0f ),
				res.x / (float)res.y,
				0.1f,
				10.0f );
			ubo.proj[1][1] *= -1;
#pragma warning( pop )

			_compute_buffers[swap_index]->unmap();
		}

		void destroy()
		{
			_compute_buffers.clear();
		}

		std::vector< std::shared_ptr< compute_buffer > > _compute_buffers;
	};


	struct instance_buffer_impl : batch_buffer_impl
	{
		void update( uint32_t swap_index )
		{
			static auto start_time = std::chrono::high_resolution_clock::now();
			auto current_time = std::chrono::high_resolution_clock::now();
			float time =
				std::chrono::duration< float, std::chrono::seconds::period >(
					current_time - start_time )
					.count();

			auto view = buffer_view< UniformBufferInstance >( 1, nullptr );
			_compute_buffers[swap_index]->map( &view );
			UniformBufferInstance& ubo = view[0] = {};

#pragma warning( push )
#pragma warning( disable : 4127 ) // constant if condition
			ubo.model = glm::rotate(
				glm::mat4( 1.0f ),
				time * glm::radians( 90.0f ),
				glm::vec3( 0.0f, 0.0f, 1.0f ) );
#pragma warning( pop )

			_compute_buffers[swap_index]->unmap();
		}
	};


	batch_buffer_impl batch = {
		this,
	};


	instance_buffer_impl instance = {
		this,
	};

	struct new_program_impl
	{
		[[nodiscard]] bool create()
		{
			std::shared_ptr< vertex_shader > vertex =
				app->_context->make_vertex_shader();
			std::shared_ptr< fragment_shader > fragment =
				app->_context->make_fragment_shader();


			if ( !app->load_buffer( VERTEX_PATH, vertex.get() ) )
			{
				return false;
			}

			if ( !app->load_buffer( FRAGMENT_PATH, fragment.get() ) )
			{
				return false;
			}
			program = app->_context->make_program( {
				"test program",
				vertex,
				fragment,
			} );

			auto vulkan = ASSERT_CAST( vulkan_program*, program.get() );
			descriptor_set_layouts = {
				vulkan->batch_parameters().cfg().vk.descriptor_set_layout,
				vulkan->material_parameters().cfg().vk.descriptor_set_layout,
				vulkan->instance_parameters().cfg().vk.descriptor_set_layout,
			};


			pipeline_layout = vulkan->pipeline_layout();
			shader_stages = {
				vulkan->cfg().vk.vertex->stage_info(),
				vulkan->cfg().vk.fragment->stage_info(),
			};

			return true;
		}

		const std::vector< VkPipelineShaderStageCreateInfo >& get_shader_stages()
		{
			return shader_stages;
		}

		void destroy()
		{
			// destroy
			program = nullptr;
		}

		HelloTriangleApplication* app;
		std::unique_ptr< program > program;
		std::array< VkDescriptorSetLayout, 3 > descriptor_set_layouts;
		VkPipelineLayout pipeline_layout;
		std::vector< VkPipelineShaderStageCreateInfo > shader_stages;
	};

	new_program_impl program_impl = {
		this,
	};

	struct new_render_states_impl
	{
		[[nodiscard]] bool create()
		{
			uint8_t color_write_mask = 0b1111;

			cull_mode c = {
				true,
				cull::BACK,
			};

			blend_mode b = {
				false,
			};

			stencil_mode s = {
				false,
			};

			depth_mode d = {
				true,
				true,
				compare::LESS,
			};

			_render_states = app->_context->make_render_states( {
				color_write_mask,
				c,
				b,
				s,
				d,
			} );

			return (bool)_render_states;
		}

		void destroy()
		{
			// destroy
		}

		VkCullModeFlags cull() const
		{
			auto* vulkan =
				ASSERT_CAST( vulkan_render_states*, _render_states.get() );
			return vulkan->cfg().vk.cull;
		}

		const VkPipelineDepthStencilStateCreateInfo* depth_stencil() const
		{
			auto* vulkan =
				ASSERT_CAST( vulkan_render_states*, _render_states.get() );
			return &vulkan->cfg().vk.depth_stencil;
		}

		const VkPipelineColorBlendAttachmentState* color_blend_attachment()
			const
		{
			auto* vulkan =
				ASSERT_CAST( vulkan_render_states*, _render_states.get() );
			return &vulkan->cfg().vk.color_blend;
		}

		HelloTriangleApplication* app;
		std::unique_ptr< render_states > _render_states;
	};

	new_render_states_impl render_states_impl = {
		this,
	};

	struct old_descriptor_set_impl
	{
		void create_descriptor_pool()
		{
			uint32_t uniform_buffers = 0;
			uint32_t storage_buffers = 0;
			uint32_t samplers = 0;

			for ( auto* parameters : {
					  &app->program_impl.program->batch_parameters(),
					  &app->program_impl.program->material_parameters(),
					  &app->program_impl.program->instance_parameters(),
				  } )
			{
				for ( int i = 0; i < parameters->count(); ++i )
				{
					const auto& cfg = parameters->parameter( i ).cfg();
					switch ( cfg.type )
					{
					case parameter::type::UNIFORM_BUFFER:
						uniform_buffers += (uint32_t)cfg.array_size;
						break;

					case parameter::type::STORAGE_BUFFER:
						storage_buffers += (uint32_t)cfg.array_size;
						break;

					case parameter::type::SAMPLER2D:
						samplers += (uint32_t)cfg.array_size;
						break;
					}
				}
			}

			uint32_t pool_type_count = 0;

			std::array< VkDescriptorPoolSize, 3 > pool_sizes = {};

			if ( uniform_buffers )
			{
				pool_sizes[pool_type_count++] = {
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					uniform_buffers * app->_swap_image_count,
				};
			}

			if ( storage_buffers )
			{
				pool_sizes[pool_type_count++] = {
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					storage_buffers * app->_swap_image_count,
				};
			}

			if ( samplers )
			{
				pool_sizes[pool_type_count++] = {
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					samplers * app->_swap_image_count,
				};
			}

			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.poolSizeCount = pool_type_count;
			pool_info.pPoolSizes = pool_sizes.data();
			pool_info.maxSets =
				static_cast< uint32_t >( app->_swap_image_count * 3 );

			vkCreateDescriptorPool(
				app->_device,
				&pool_info,
				nullptr,
				&_descriptor_pool );
		}

		void create_descriptor_sets()
		{
			auto* program =
				ASSERT_CAST( vulkan_program*, app->program_impl.program.get() );

			_batch_descriptor_sets.resize( app->_swap_image_count );
			_material_descriptor_sets.resize( app->_swap_image_count );
			_instance_descriptor_sets.resize( app->_swap_image_count );

			allocate_descriptor_set(
				app->_device,
				_descriptor_pool,
				program->batch_parameters().cfg().vk.descriptor_set_layout,
				&_batch_descriptor_sets );
			allocate_descriptor_set(
				app->_device,
				_descriptor_pool,
				program->material_parameters().cfg().vk.descriptor_set_layout,
				&_material_descriptor_sets );
			allocate_descriptor_set(
				app->_device,
				_descriptor_pool,
				program->instance_parameters().cfg().vk.descriptor_set_layout,
				&_instance_descriptor_sets );

			for ( size_t i = 0; i < app->_swap_image_count; i++ )
			{
				VkDescriptorBufferInfo batch_info =
					app->batch.descriptor_info( i );

				VkDescriptorImageInfo material_info =
					app->texture_impl.descriptor_info();

				VkDescriptorBufferInfo instance_info =
					app->instance.descriptor_info( i );

				std::array< VkWriteDescriptorSet, 3 > descriptor_writes = {};

				descriptor_writes[0].sType =
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptor_writes[0].dstSet = _batch_descriptor_sets[i];
				descriptor_writes[0].dstBinding = 1;
				descriptor_writes[0].dstArrayElement = 0;
				descriptor_writes[0].descriptorType =
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				descriptor_writes[0].descriptorCount = 1;
				descriptor_writes[0].pBufferInfo = &batch_info;

				descriptor_writes[1].sType =
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptor_writes[1].dstSet = _material_descriptor_sets[i];
				descriptor_writes[1].dstBinding = 10;
				descriptor_writes[1].dstArrayElement = 0;
				descriptor_writes[1].descriptorType =
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptor_writes[1].descriptorCount = 1;
				descriptor_writes[1].pImageInfo = &material_info;

				descriptor_writes[2].sType =
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptor_writes[2].dstSet = _instance_descriptor_sets[i];
				descriptor_writes[2].dstBinding = 3;
				descriptor_writes[2].dstArrayElement = 0;
				descriptor_writes[2].descriptorType =
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				descriptor_writes[2].descriptorCount = 1;
				descriptor_writes[2].pBufferInfo = &instance_info;

				vkUpdateDescriptorSets(
					app->_device,
					static_cast< uint32_t >( descriptor_writes.size() ),
					descriptor_writes.data(),
					0,
					nullptr );
			}
		}

		void create()
		{
			create_descriptor_pool();
			create_descriptor_sets();
		}

		void destroy()
		{
			vkDestroyDescriptorPool( app->_device, _descriptor_pool, nullptr );
		}

		HelloTriangleApplication* app = nullptr;
		VkDescriptorPool _descriptor_pool;
		std::vector< VkDescriptorSet > _batch_descriptor_sets;
		std::vector< VkDescriptorSet > _material_descriptor_sets;
		std::vector< VkDescriptorSet > _instance_descriptor_sets;
	};


	// struct new_descriptor_set_impl
	//{
	//	void create_descriptor_pool()
	//	{
	//		uint32_t uniform_buffers = 0;
	//		uint32_t storage_buffers = 0;
	//		uint32_t samplers = 0;

	//		for ( auto* parameters : {
	//				  &app->program_impl.program->batch_parameters(),
	//				  &app->program_impl.program->material_parameters(),
	//				  &app->program_impl.program->instance_parameters(),
	//			  } )
	//		{
	//			for ( int i = 0; i < parameters->count(); ++i )
	//			{
	//				const auto& cfg = parameters->parameter( i ).cfg();
	//				switch ( cfg.type )
	//				{
	//				case parameter::type::UNIFORM_BUFFER:
	//					uniform_buffers += (uint32_t)cfg.array_size;
	//					break;

	//				case parameter::type::STORAGE_BUFFER:
	//					storage_buffers += (uint32_t)cfg.array_size;
	//					break;

	//				case parameter::type::SAMPLER2D:
	//					samplers += (uint32_t)cfg.array_size;
	//					break;
	//				}
	//			}
	//		}

	//		uint32_t pool_type_count = 0;

	//		std::array< VkDescriptorPoolSize, 3 > pool_sizes = {};

	//		if ( uniform_buffers )
	//		{
	//			pool_sizes[pool_type_count++] = {
	//				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	//				uniform_buffers * app->_swap_image_count,
	//			};
	//		}

	//		if ( storage_buffers )
	//		{
	//			pool_sizes[pool_type_count++] = {
	//				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
	//				storage_buffers * app->_swap_image_count,
	//			};
	//		}

	//		if ( samplers )
	//		{
	//			pool_sizes[pool_type_count++] = {
	//				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	//				samplers * app->_swap_image_count,
	//			};
	//		}

	//		VkDescriptorPoolCreateInfo pool_info = {};
	//		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	//		pool_info.poolSizeCount = pool_type_count;
	//		pool_info.pPoolSizes = pool_sizes.data();
	//		pool_info.maxSets =
	//			static_cast< uint32_t >( app->_swap_image_count * 3 );

	//		vkCreateDescriptorPool(
	//			app->_device,
	//			&pool_info,
	//			nullptr,
	//			&_descriptor_pool );
	//	}

	//	void create_descriptor_sets()
	//	{
	//		auto* program =
	//			ASSERT_CAST( vulkan_program*, app->program_impl.program.get() );

	//		_batch_descriptor_sets.resize( app->_swap_image_count );
	//		_material_descriptor_sets.resize( app->_swap_image_count );
	//		_instance_descriptor_sets.resize( app->_swap_image_count );

	//		allocate_descriptor_set(
	//			app->_device,
	//			_descriptor_pool,
	//			program->batch_parameters().cfg().vk.descriptor_set_layout,
	//			&_batch_descriptor_sets );
	//		allocate_descriptor_set(
	//			app->_device,
	//			_descriptor_pool,
	//			program->material_parameters().cfg().vk.descriptor_set_layout,
	//			&_material_descriptor_sets );
	//		allocate_descriptor_set(
	//			app->_device,
	//			_descriptor_pool,
	//			program->instance_parameters().cfg().vk.descriptor_set_layout,
	//			&_instance_descriptor_sets );

	//		for ( size_t i = 0; i < app->_swap_image_count; i++ )
	//		{
	//			VkDescriptorBufferInfo batch_info =
	//				app->batch.descriptor_info( i );

	//			VkDescriptorImageInfo material_info =
	//				app->texture_impl.descriptor_info();

	//			VkDescriptorBufferInfo instance_info =
	//				app->instance.descriptor_info( i );

	//			std::array< VkWriteDescriptorSet, 3 > descriptor_writes = {};

	//			descriptor_writes[0].sType =
	//				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//			descriptor_writes[0].dstSet = _batch_descriptor_sets[i];
	//			descriptor_writes[0].dstBinding = 1;
	//			descriptor_writes[0].dstArrayElement = 0;
	//			descriptor_writes[0].descriptorType =
	//				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	//			descriptor_writes[0].descriptorCount = 1;
	//			descriptor_writes[0].pBufferInfo = &batch_info;

	//			descriptor_writes[1].sType =
	//				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//			descriptor_writes[1].dstSet = _material_descriptor_sets[i];
	//			descriptor_writes[1].dstBinding = 10;
	//			descriptor_writes[1].dstArrayElement = 0;
	//			descriptor_writes[1].descriptorType =
	//				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//			descriptor_writes[1].descriptorCount = 1;
	//			descriptor_writes[1].pImageInfo = &material_info;

	//			descriptor_writes[2].sType =
	//				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//			descriptor_writes[2].dstSet = _instance_descriptor_sets[i];
	//			descriptor_writes[2].dstBinding = 3;
	//			descriptor_writes[2].dstArrayElement = 0;
	//			descriptor_writes[2].descriptorType =
	//				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	//			descriptor_writes[2].descriptorCount = 1;
	//			descriptor_writes[2].pBufferInfo = &instance_info;

	//			vkUpdateDescriptorSets(
	//				app->_device,
	//				static_cast< uint32_t >( descriptor_writes.size() ),
	//				descriptor_writes.data(),
	//				0,
	//				nullptr );
	//		}
	//	}

	//	HelloTriangleApplication* app = nullptr;

	//	using primitives_t = std::shared_ptr< primitives >;
	//	using primitives_set_t = std::vector< primitives_t >;

	//	struct batch_node_prims
	//	{
	//		primitives_set_t primitives;

	//		VkDevice device;
	//		VkDescriptorPool _descriptor_pool;
	//		std::vector< VkDescriptorSet > _descriptor_sets;
	//		primitives_set_t instance_primitives;

	//		[[nodiscard]] bool initialize( const vulkan_parameters& parameters )
	//		{
	//			vulkan_primitive_sequencer sequencer;
	//			for ( const auto& primitives : primitives )
	//			{
	//				if ( false == sequencer.reset( parameters, *primitives ) )
	//				{
	//					return false;
	//				}
	//				"not finished"
	//			}
	//		}

	//		~batch_node_prims()
	//		{
	//			vkDestroyDescriptorPool( device, _descriptor_pool, nullptr );
	//		}
	//	};

	//	batch_node_prims _batch_inputs;
	//	batch_node_prims _material_inputs;
	//	batch_node_prims _instance_inputs;

	//	void create()
	//	{
	//		for ( size_t i = 0; i < app->_swap_image_count; ++i )
	//		{
	//			_batch_inputs.primitives.push_back(
	//				app->_context->make_primitives( { {
	//					{ "batch", app->batch._compute_buffers[i] },
	//				} } ) );

	//			_material_inputs.primitives.push_back(
	//				app->_context->make_primitives( { {
	//					{ "texSampler", app->texture_impl.texture },
	//				} } ) );

	//			_instance_inputs.primitives.push_back(
	//				app->_context->make_primitives( { {
	//					{ "instance", app->instance._compute_buffers[i] },
	//				} } ) );
	//		}
	//		create_descriptor_pool();
	//		create_descriptor_sets();
	//	}

	//	void destroy()
	//	{
	//		vkDestroyDescriptorPool( app->_device, _descriptor_pool, nullptr );
	//	}
	//};

	old_descriptor_set_impl descriptor_set_impl = { this };

	VkCommandPool _command_pool;

	std::vector< VkCommandBuffer > _command_buffers;

	const int MAX_FRAMES_IN_FLIGHT = 2;
	std::vector< VkSemaphore > _image_available_semaphores;
	std::vector< VkSemaphore > _render_finished_semaphores;
	std::vector< VkFence > _in_flight_fences;
	VkSampleCountFlagBits _msaa_samples;
	uint32_t _swap_image_count = 0;
	size_t _current_frame = 0;

	bool _framebuffer_resized = false;

public:
	~HelloTriangleApplication()
	{
		_present_queue = nullptr;
		_graphics_queue = nullptr;
		_context = nullptr;
	}

	bool init_vulkan( const scoped_ptr< vulkan_context >& context )
	{
		_context = context;

		_window = &_context->window();
		_instance = _context->instance();
		_physical_device = _context->physical_device();
		_device = _context->device();
		_present_queue = _context->synchronization().cfg().present_queue;
		_graphics_queue = _context->synchronization().cfg().graphics_queue;
		_swap_image_count =
			(uint32_t)_context->back_buffer().framebuffers().size();
		_msaa_samples = _context->back_buffer().cfg().vk.sample_count;

		if ( batch.create() && instance.create() && texture_impl.create() &&
			 program_impl.create() && render_states_impl.create() &&
			 geo_impl.create() )
		{
			create_command_pool();
			create_graphics_pipeline();
			descriptor_set_impl.create();

			if ( load_model() )
			{
				create_command_buffers();
				create_sync_objects();
				return true;
			}
		}

		return false;
	}

	bool main_loop()
	{
		if ( _window->poll_events() )
		{
			draw_frame();
			return true;
		}
		return false;
	}

	void cleanup()
	{
		on_cleanup_swap_chain();

		texture_impl.destroy();
		program_impl.destroy();

		descriptor_set_impl.destroy();

		batch.destroy();
		instance.destroy();

		geo_impl.destroy();

		for ( auto* semaphores : {
				  &_render_finished_semaphores,
				  &_image_available_semaphores,
			  } )
		{
			for ( VkSemaphore semaphore : *semaphores )
			{
				vkDestroySemaphore( _device, semaphore, nullptr );
			}
		}
		for ( VkFence fence : _in_flight_fences )
		{
			vkDestroyFence( _device, fence, nullptr );
		}

		vkDestroyCommandPool( _device, _command_pool, nullptr );
	}

private:
	void on_cleanup_swap_chain()
	{
		vkDeviceWaitIdle( _device );

		vkDestroyPipeline( _device, _graphics_pipeline, nullptr );
		vkFreeCommandBuffers(
			_device,
			_command_pool,
			static_cast< uint32_t >( _command_buffers.size() ),
			_command_buffers.data() );
	}

	void on_recreate_swap_chain()
	{
		glm::ivec2 window_res = _window->res();

		while ( window_res.x == 0 || window_res.y == 0 )
		{
			_window->poll_events();
			window_res = _window->res();
			std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
		}

		on_cleanup_swap_chain();
		_context->resize_back_buffer( window_res );
		create_graphics_pipeline();
		create_command_buffers();
	}

	void create_command_pool()
	{
		VkCommandPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.queueFamilyIndex = _graphics_queue->cfg().family_index;

		vkCreateCommandPool( _device, &pool_info, nullptr, &_command_pool );
	}

	void create_graphics_pipeline()
	{
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)_context->back_buffer().cfg().res.x;
		viewport.height = (float)_context->back_buffer().cfg().res.y;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = {};
		scissor.extent = {
			(uint32_t)_context->back_buffer().cfg().res.x,
			(uint32_t)_context->back_buffer().cfg().res.y,
		};

		VkPipelineViewportStateCreateInfo viewport_state = {};
		viewport_state.sType =
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state.viewportCount = 1;
		viewport_state.pViewports = &viewport;
		viewport_state.scissorCount = 1;
		viewport_state.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType =
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = render_states_impl.cull();
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType =
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = _msaa_samples;

		VkPipelineColorBlendStateCreateInfo color_blending = {};
		color_blending.sType =
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blending.logicOpEnable = VK_FALSE;
		color_blending.logicOp = VK_LOGIC_OP_COPY;
		color_blending.attachmentCount = 1;
		color_blending.pAttachments =
			render_states_impl.color_blend_attachment();
		color_blending.blendConstants[0] = 0.0f;
		color_blending.blendConstants[1] = 0.0f;
		color_blending.blendConstants[2] = 0.0f;
		color_blending.blendConstants[3] = 0.0f;


		const auto& shader_stages = program_impl.get_shader_stages();

		vulkan_program* program =
			ASSERT_CAST( vulkan_program*, program_impl.program.get() );
		vulkan_geometry* geometry =
			ASSERT_CAST( vulkan_geometry*, geo_impl.geometry.get() );


		vulkan_attribute_sequencer sequencer;
		if ( !sequencer.reset( program->vertex_parameters(), *geometry ) )
		{
			return;
		}

		VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
		input_assembly.sType =
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		input_assembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
		vertex_input_info.sType =
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		vertex_input_info.vertexBindingDescriptionCount =
			sequencer.binding_description_count();
		vertex_input_info.pVertexBindingDescriptions =
			sequencer.binding_descriptions();

		vertex_input_info.vertexAttributeDescriptionCount =
			(uint32_t)program->vertex_parameters().count();
		vertex_input_info.pVertexAttributeDescriptions =
			sequencer.attribute_descriptions();

		VkGraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = (uint32_t)shader_stages.size();
		pipeline_info.pStages = shader_stages.data();
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &input_assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisampling;
		pipeline_info.pDepthStencilState = render_states_impl.depth_stencil();
		pipeline_info.pColorBlendState = &color_blending;
		pipeline_info.layout = program_impl.pipeline_layout;
		pipeline_info.renderPass = _context->back_buffer().render_pass();
		;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

		vkCreateGraphicsPipelines(
			_device,
			VK_NULL_HANDLE,
			1,
			&pipeline_info,
			nullptr,
			&_graphics_pipeline );
	}

	[[nodiscard]] bool load_buffer(
		const std::string& path,
		buffer* out_buffer )
	{
		const char* err = nullptr;
		std::ifstream ifs( path.c_str(), std::ios::binary | std::ios::ate );

		if ( !out_buffer )
		{
			err = "out buffer is null";
		}
		else if ( !ifs )
		{
			err = "failed to find file";
		}
		else
		{
			buffer_view< char > view( ifs.tellg(), nullptr );
			out_buffer->map( &view );

			if ( nullptr == view.data() )
			{
				err =
					"could not obtain mapped memory from buffer to copy file "
					"into";

				ifs.close();
			}
			else
			{
				ifs.seekg( 0, std::ios::beg );
				ifs.read( view.data(), view.byte_size() );
				ifs.close();

				out_buffer->unmap();
			}
		}

		if ( err )
		{
			LOG_CRITICAL( "%s : file '%s'", err, path.c_str() );
		}


		return true;
	}

	[[nodiscard]] bool load_model()
	{
		std::vector< Vertex > vertices;
		std::vector< uint32_t > indices;

		tinyobj::attrib_t attrib;
		std::vector< tinyobj::shape_t > shapes;
		std::vector< tinyobj::material_t > materials;
		std::string warn, err;

		if ( !tinyobj::LoadObj(
				 &attrib,
				 &shapes,
				 &materials,
				 &warn,
				 &err,
				 MODEL_PATH.c_str() ) )
		{
			if ( !warn.empty() )
			{
				LOG_WARNING( "tinyobj :", warn.c_str() );
			}
			if ( !err.empty() )
			{
				LOG_CRITICAL( "tinyobj :", err.c_str() );
			}

			return false;
		}

		std::unordered_map< Vertex, size_t > unique_vertices = {};

		for ( const auto& shape : shapes )
		{
			for ( const auto& index : shape.mesh.indices )
			{
				auto emplace = unique_vertices.emplace(
					Vertex( {
						{
							// pos
							attrib.vertices[3 * index.vertex_index + 0],
							attrib.vertices[3 * index.vertex_index + 1],
							attrib.vertices[3 * index.vertex_index + 2],
						},
						{
							// color
							1.0f,
							1.0f,
							1.0f,
						},
						{
							// texcorrd
							attrib.texcoords[2 * index.texcoord_index + 0],
							1 - attrib.texcoords[2 * index.texcoord_index + 1],
						},
					} ),
					unique_vertices.size() );

				if ( emplace.second )
				{
					vertices.push_back( emplace.first->first );
				}

				indices.push_back( (uint32_t)emplace.first->second );
			}
		}

		return geo_impl.upload( vertices, indices );
	}


	void create_command_buffers()
	{
		_command_buffers.resize( _swap_image_count );

		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.commandPool = _command_pool;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandBufferCount = (uint32_t)_command_buffers.size();

		vkAllocateCommandBuffers(
			_device,
			&alloc_info,
			_command_buffers.data() );

		for ( size_t i = 0; i < _command_buffers.size(); i++ )
		{
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

			vkBeginCommandBuffer( _command_buffers[i], &begin_info );

			VkRenderPassBeginInfo render_pass_info = {};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = _context->back_buffer().render_pass();
			render_pass_info.framebuffer =
				_context->back_buffer().framebuffers()[i];
			render_pass_info.renderArea.offset = {};
			render_pass_info.renderArea.extent = {
				(uint32_t)_context->back_buffer().cfg().res.x,
				(uint32_t)_context->back_buffer().cfg().res.y,
			};

			std::array< VkClearValue, 2 > clear_values = {};
			clear_values[0].color = {
				0.0f,
				0.0f,
				0.0f,
				1.0f,
			};
			clear_values[1].depthStencil = {
				1.0f,
				0,
			};

			render_pass_info.clearValueCount =
				static_cast< uint32_t >( clear_values.size() );
			render_pass_info.pClearValues = clear_values.data();

			vkCmdBeginRenderPass(
				_command_buffers[i],
				&render_pass_info,
				VK_SUBPASS_CONTENTS_INLINE );

			vkCmdBindPipeline(
				_command_buffers[i],
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				_graphics_pipeline );

			std::array< VkDescriptorSet, 3 > descriptor_sets = {
				descriptor_set_impl._batch_descriptor_sets[i],
				descriptor_set_impl._material_descriptor_sets[i],
				descriptor_set_impl._instance_descriptor_sets[i],
			};

			vkCmdBindDescriptorSets(
				_command_buffers[i],
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				program_impl.pipeline_layout,
				0,
				(uint32_t)descriptor_sets.size(),
				descriptor_sets.data(),
				0,
				nullptr );

			geo_impl.draw( _command_buffers[i] );

			vkCmdEndRenderPass( _command_buffers[i] );

			vkEndCommandBuffer( _command_buffers[i] );
		}
	}

	void create_sync_objects()
	{
		_image_available_semaphores.resize( MAX_FRAMES_IN_FLIGHT );
		_render_finished_semaphores.resize( MAX_FRAMES_IN_FLIGHT );
		_in_flight_fences.resize( MAX_FRAMES_IN_FLIGHT );

		VkSemaphoreCreateInfo semaphore_info = {};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fence_info = {};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
		{
			vkCreateSemaphore(
				_device,
				&semaphore_info,
				nullptr,
				&_image_available_semaphores[i] );
			vkCreateSemaphore(
				_device,
				&semaphore_info,
				nullptr,
				&_render_finished_semaphores[i] );
			vkCreateFence(
				_device,
				&fence_info,
				nullptr,
				&_in_flight_fences[i] );
		}
	}

	void draw_frame()
	{
		vkWaitForFences(
			_device,
			1,
			&_in_flight_fences[_current_frame],
			VK_TRUE,
			std::numeric_limits< uint64_t >::max() );

		uint32_t image_index = 0;

		switch ( vkAcquireNextImageKHR(
			_device,
			_context->back_buffer().swap_chain(),
			std::numeric_limits< uint64_t >::max(),
			_image_available_semaphores[_current_frame],
			VK_NULL_HANDLE,
			&image_index ) )
		{
		case VK_SUBOPTIMAL_KHR:
			LOG_WARNING( "vkAcquireNextImageKHR: VK_SUBOPTIMAL_KHR" );
			LOG_WARNING( "vkAcquireNextImageKHR: VK_SUBOPTIMAL_KHR" );
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			on_recreate_swap_chain();
			break;
		}

		vkQueueWaitIdle( _present_queue->get() );

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore wait_semaphores[] = {
			_image_available_semaphores[_current_frame],
		};
		VkPipelineStageFlags wait_stages[] = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		};

		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;


		batch.update( image_index );
		instance.update( image_index );

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &_command_buffers[image_index];

		VkSemaphore signal_semaphores[] = {
			_render_finished_semaphores[_current_frame],
		};

		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = signal_semaphores;

		vkResetFences( _device, 1, &_in_flight_fences[_current_frame] );

		vkQueueSubmit(
			_graphics_queue->get(),
			1,
			&submit_info,
			_in_flight_fences[_current_frame] );

		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = signal_semaphores;

		VkSwapchainKHR swap_chains[] = {
			_context->back_buffer().swap_chain(),
		};

		present_info.swapchainCount = 1;
		present_info.pSwapchains = swap_chains;

		present_info.pImageIndices = &image_index;

		switch ( vkQueuePresentKHR( _present_queue->get(), &present_info ) )
		{
		case VK_SUBOPTIMAL_KHR:
			LOG_WARNING( "vkAcquireNextImageKHR: VK_SUBOPTIMAL_KHR" );
			[[fallthrough]];
		case VK_ERROR_OUT_OF_DATE_KHR:
			_framebuffer_resized = true;
		}

		if ( _framebuffer_resized )
		{
			_framebuffer_resized = false;
			on_recreate_swap_chain();
		}

		_current_frame = ( _current_frame + 1 ) % MAX_FRAMES_IN_FLIGHT;
	}
};


HelloTriangleApplication hello_triangle_application;

void port_example();

bool test_init_vulkan_context( const scoped_ptr< vulkan_context >& context )
{
	return hello_triangle_application.init_vulkan( context );
}

bool test_loop_vulkan_context()
{
	return hello_triangle_application.main_loop();
}

void test_cleanup_vulkan_context()
{
	hello_triangle_application.cleanup();
}
