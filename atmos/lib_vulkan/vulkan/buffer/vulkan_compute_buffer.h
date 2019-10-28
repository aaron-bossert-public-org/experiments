
#pragma once

#include <igpu/buffer/compute_buffer.h>
#include <igpu/utility/scoped_ptr.h>

#include <vulkan/defines/vulkan_includes.h>

namespace igpu
{
	class vulkan_buffer_mediator;

	class vulkan_compute_buffer : public compute_buffer
	{
	public:

		static std::unique_ptr<vulkan_compute_buffer> make(
			const config&,
			const scoped_ptr < vulkan_buffer_mediator >&);

		virtual VkBuffer get() const = 0;

	protected:

		vulkan_compute_buffer() = default;
	};
}
