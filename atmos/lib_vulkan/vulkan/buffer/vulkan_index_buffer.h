
#pragma once

#include <igpu/buffer/index_buffer.h>
#include <igpu/buffer/index_format.h>
#include <igpu/utility/scoped_ptr.h>

#include <vulkan/defines/vulkan_includes.h>

namespace igpu
{
	VkIndexType to_vulkan_format(index_format format);

	class vulkan_buffer_mediator;

	class vulkan_index_buffer : public index_buffer
	{
	public:

		static std::unique_ptr<vulkan_index_buffer> make(
			const config&,
			const scoped_ptr < vulkan_buffer_mediator >&);

		virtual VkBuffer get() = 0;

		VkIndexType format();

	protected:

		vulkan_index_buffer(VkIndexType);

	private:

		const VkIndexType _format;
	};
}
