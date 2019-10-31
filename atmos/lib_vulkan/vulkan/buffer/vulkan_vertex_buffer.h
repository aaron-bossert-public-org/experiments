
#pragma once

#include <framework/utility/scoped_ptr.h>

#include <igpu/buffer/vertex_buffer.h>

#include <vulkan/defines/vulkan_includes.h>

namespace igpu
{
	class vulkan_buffer_mediator;

	class vulkan_vertex_buffer : public vertex_buffer
	{
	public:

		static std::unique_ptr<vulkan_vertex_buffer> make(
			const config&,
			const scoped_ptr< vulkan_buffer_mediator >&);

		virtual VkBuffer get() const = 0;

	protected:

		vulkan_vertex_buffer() = default;
	};
}
