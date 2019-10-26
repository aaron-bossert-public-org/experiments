
#pragma once


#include <vulkan/buffer/vulkan_buffer.h>
#include <vulkan/texture/vulkan_texture2d.h>
#include <igpu/utility/scoped_ptr.h>
#include <framework/perf/metrics.h>

namespace igpu
{
	class vulkan_buffer;
	class vulkan_buffer_mediator;
	class vulkan_image;

	class vulkan_staged_texture2d : public vulkan_texture2d
	{
	public:

		const config& cfg() const override;

		vulkan_staged_texture2d(
			const config& cfg,
			const scoped_ptr < vulkan_buffer_mediator >&);

		~vulkan_staged_texture2d() override;

		void map(size_t, buffer_view_base*) override;

		void unmap(
			const state& state) override;

		void unmap() override;

		const state& current_state() const override;

		size_t byte_size() const override;

		vulkan_image& image() override;

	private:

		void unmap(
			const buffer_view<char>&,
			const state&);

	private:

		const config _cfg;
		state _state = {};
		scoped_ptr < vulkan_buffer_mediator > _buffer_mediator;
		buffer_view<char> _mapped_view = {};
		vulkan_buffer _staging_buffer;
		std::unique_ptr<vulkan_image> _image;
	};
}
