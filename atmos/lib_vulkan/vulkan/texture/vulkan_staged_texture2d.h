
#pragma once


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

		vulkan_staged_texture2d(
			const config& cfg,
			const scoped_ptr < vulkan_buffer_mediator >&);

		~vulkan_staged_texture2d() override;

		void map(size_t, buffer_view_base*) override;

		void unmap_explicit(
			const glm::ivec2&,
			texture_format) override;

		void unmap_raw_file() override;

		const config& cfg() const override;

		source_type source() const override;

		const glm::ivec2& res() const override;

		texture_format format() const override;

		vulkan_image& image() override;

	private:

		void unmap(
			const buffer_view<char>&,
			const glm::ivec2&,
			texture_format,
			size_t mipmap_count);

	private:

		const config _cfg;
		scoped_ptr < vulkan_buffer_mediator > _buffer_mediator;
		
		struct
		{
			source_type source = source_type::UNDEFINED;
			glm::ivec2 res = {};
			texture_format format = texture_format::UNDEFINED;
			buffer_view<char> mapped_view = {};
			std::unique_ptr<vulkan_buffer> staging_buffer;
			std::unique_ptr<vulkan_image> image;
		} _state;
	};
}
