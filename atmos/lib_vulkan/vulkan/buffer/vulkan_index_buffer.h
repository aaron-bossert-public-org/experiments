
#pragma once

#include <igpu/buffer/index_buffer.h>
#include <igpu/buffer/index_format.h>

#include <vulkan/buffer/vulkan_staged_buffer.h>

namespace igpu
{
	VkIndexType to_vulkan_format(index_format format);

	class vulkan_index_buffer : public vulkan_staged_buffer_t < index_buffer >
	{
	public:

		static std::unique_ptr<vulkan_index_buffer> make(
			const config&,
			const scoped_ptr < vulkan_buffer_mediator >&);

		const config& cfg() const override;

		VkIndexType vulkan_index_buffer::format();

	private:

		vulkan_index_buffer(
			const config&,
			const vulkan_staged_buffer::config&);

	private:

		const config _cfg;
	};
}
