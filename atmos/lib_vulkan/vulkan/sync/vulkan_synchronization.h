
#pragma once

#include <memory>
#include <vector>

#include <framework/utility/scoped_ptr.h>

#include <igpu/buffer/compute_buffer.h>
#include <igpu/buffer/index_buffer.h>
#include <igpu/buffer/vertex_buffer.h>

#include <vulkan/defines/vulkan_includes.h>

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
			scoped_ptr<vulkan_queue> present_queue;
			scoped_ptr<vulkan_queue> graphics_queue;
			scoped_ptr<vulkan_queue> compute_queue;
			scoped_ptr<vulkan_queue> transfer_queue;
		};

		const config& cfg() const;

		VmaAllocator vma();

		void copy(
			vulkan_buffer& src,
			vulkan_buffer& dst,
			uint32_t size,
			uint32_t src_offset = 0,
			uint32_t dst_offset = 0);

		void copy(
			vulkan_buffer& src,
			vulkan_image& dst,
			uint32_t src_offset = 0);

		bool can_generate_mipmaps(
			VkFormat format,
			VkImageTiling tiling);

		void generate_mipmaps(
			vulkan_image&);

		~vulkan_synchronization();

		static std::unique_ptr<vulkan_synchronization> make(const config&);

	protected:

		vulkan_synchronization(
			const config&,
			VmaAllocator);

	private:

		const config _cfg;
		VmaAllocator _vma;
	};
}
