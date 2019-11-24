
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/buffer/vertex_buffer.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_buffer;
	class vulkan_managers;

	class vulkan_vertex_buffer : public vertex_buffer
	{
	public:
		struct config : vertex_buffer::config
		{
			struct vulkan
			{
				VkDevice device = nullptr;
				VmaAllocator vma = nullptr;
				scoped_ptr< vulkan_managers > managers;
			};

			vulkan vk;
		};

		const config& cfg() const override;

		virtual vulkan_buffer& gpu_object() = 0;

		virtual const vulkan_buffer& gpu_object() const = 0;

		virtual size_t byte_size() const = 0;

		static std::unique_ptr< vulkan_vertex_buffer > make( const config& );

	protected:
		vulkan_vertex_buffer( const config& );

	private:
		const config _cfg;
	};
}
