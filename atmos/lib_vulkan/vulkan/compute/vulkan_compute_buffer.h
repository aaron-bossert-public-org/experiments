
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/compute/compute_buffer.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_buffer;
	class vulkan_managers;

	class vulkan_compute_buffer : public compute_buffer
	{
	public:
		struct config : compute_buffer::config
		{
			struct vulkan
			{
				VkDevice device = nullptr;
				VmaAllocator vma = nullptr;
				scoped_ptr< vulkan_managers > managers;
			};

			vulkan vk;
		};

		virtual const config& cfg() const = 0;

		virtual vulkan_buffer& gpu_object() = 0;

		virtual const vulkan_buffer& gpu_object() const = 0;

		virtual size_t byte_size() const = 0;

		static std::unique_ptr< vulkan_compute_buffer > make( const config& );
	};
}
