
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/buffer/compute_buffer.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_buffer;
	class vulkan_synchronization;

	class vulkan_compute_buffer : public compute_buffer
	{
	public:
		virtual vulkan_buffer& gpu_resource() = 0;

		virtual const vulkan_buffer& gpu_resource() const = 0;

		static std::unique_ptr< vulkan_compute_buffer > make(
			const config&,
			const scoped_ptr< vulkan_synchronization >& );
	};
}
