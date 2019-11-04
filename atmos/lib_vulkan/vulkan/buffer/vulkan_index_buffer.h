
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/buffer/index_buffer.h"
#include "igpu/buffer/index_format.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	VkIndexType to_vulkan_format( index_format format );

	class vulkan_buffer;
	class vulkan_synchronization;

	class vulkan_index_buffer : public index_buffer
	{
	public:
		struct config : index_buffer::config
		{
			struct vulkan
			{
				VkIndexType format;
			};

			vulkan vk;
		};

		virtual const config& cfg() const = 0;

		virtual vulkan_buffer& gpu_resource() = 0;

		virtual const vulkan_buffer& gpu_resource() const = 0;

		static std::unique_ptr< vulkan_index_buffer > make(
			const config&,
			const scoped_ptr< vulkan_synchronization >& );
	};
}
