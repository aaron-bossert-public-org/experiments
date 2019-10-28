
#pragma once

#include <igpu/buffer/vertex_buffer.h>
#include <igpu/utility/scoped_ptr.h>

#include <vulkan/defines/vulkan_includes.h>

namespace igpu
{
	class vulkan_buffer_mediator;

	VkFormat to_vulkan_format(components);

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
