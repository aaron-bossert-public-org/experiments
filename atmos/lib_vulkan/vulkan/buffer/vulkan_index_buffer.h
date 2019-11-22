
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/buffer/index_buffer.h"
#include "igpu/buffer/index_format.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	VkIndexType to_vulkan_format( index_format format );

	class vulkan_buffer;
	class vulkan_barrier_manager;
	class vulkan_queue_manager;

	class vulkan_index_buffer : public index_buffer
	{
	public:
		struct config : index_buffer::config
		{
			struct vulkan
			{
				VkDevice device = nullptr;
				const VkPhysicalDeviceProperties* device_properties = nullptr;
				VmaAllocator vma = nullptr;
				VkIndexType index_type;
			};

			vulkan vk;
		};

		size_t element_count() const override;

		const config& cfg() const override;

		virtual vulkan_buffer& gpu_object() = 0;

		virtual const vulkan_buffer& gpu_object() const = 0;

		virtual size_t byte_size() const = 0;

		static std::unique_ptr< vulkan_index_buffer > make(
			const config&,
			const scoped_ptr< vulkan_queue_manager >&,
			const scoped_ptr< vulkan_barrier_manager >& );

	protected:
		vulkan_index_buffer( const config& );

	private:
		const config _cfg;
	};
}
