
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/buffer/compute_buffer.h"
#include "igpu/buffer/index_buffer.h"
#include "igpu/buffer/vertex_buffer.h"

#include "framework/utility/scoped_ptr.h"

#include <memory>
#include <vector>

namespace igpu
{
	class vulkan_buffer;
	class vulkan_image;
	class vulkan_queue;

	class vulkan_synchronization
	{
	public:
		struct config
		{
			VkPhysicalDevice physical_device;
			VkDevice device;
			scoped_ptr< vulkan_queue > present_queue;
			scoped_ptr< vulkan_queue > graphics_queue;
			scoped_ptr< vulkan_queue > compute_queue;
			scoped_ptr< vulkan_queue > transfer_queue;
		};

		const config& cfg() const;

		size_t compact_queue_family_index( uint32_t queue_family_index ) const;

		const std::vector< scoped_ptr< vulkan_queue > >& compact_queues() const;

		VmaAllocator vma();

		void copy(
			vulkan_buffer& src,
			vulkan_buffer& dst,
			uint32_t size,
			uint32_t src_offset = 0,
			uint32_t dst_offset = 0 );

		void copy(
			vulkan_buffer& src,
			vulkan_image& dst,
			uint32_t src_offset = 0 );

		bool can_generate_mipmaps( VkFormat format, VkImageTiling tiling );

		void generate_mipmaps( vulkan_image& );

		~vulkan_synchronization();

		static std::unique_ptr< vulkan_synchronization > make( const config& );

	protected:
		vulkan_synchronization(
			const config&,
			VmaAllocator,
			const std::vector< size_t >&,
			const std::vector< scoped_ptr< vulkan_queue > >& );

	private:
		const config _cfg;
		const std::vector< size_t > _compact_queue_family_indices;
		const std::vector< scoped_ptr< vulkan_queue > > _compact_queues;
		VmaAllocator _vma;
	};
}
