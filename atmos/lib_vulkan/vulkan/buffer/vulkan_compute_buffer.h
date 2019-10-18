
#pragma once

#include <igpu/buffer/compute_buffer.h>

#include <vulkan/buffer/vulkan_staged_buffer.h>

namespace igpu
{
	class vulkan_compute_buffer : public vulkan_staged_buffer_t < compute_buffer >
	{
	public:

		static std::unique_ptr<vulkan_compute_buffer> make(
			const config&,
			const std::weak_ptr < vulkan_buffer_mediator >&);

	private:

		vulkan_compute_buffer(
			const config&,
			const vulkan_staged_buffer::config&);
	};
}
