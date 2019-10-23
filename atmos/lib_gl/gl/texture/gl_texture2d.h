
#pragma once


#include <igpu/texture/texture2d.h>
#include <framework/perf/metrics.h>
#include <glm/vec2.hpp>

namespace igpu
{
	class gl_texture2d : public texture2d
	{
	public:

		unsigned gl_handle() const;

		~gl_texture2d() override;

		void map(size_t, buffer_view_base*) override;

		void unmap_explicit(
			const glm::ivec2&,
			texture_format) override;

		void unmap_raw_file() override;

		const config& cfg() const override;

		source_type source() const override;

		const glm::ivec2& res() const override;

		texture_format format() const override;

		static std::unique_ptr<gl_texture2d> make(const config&);

	private:

		void upload(
			const buffer_view<char>&,
			const glm::ivec2&,
			texture_format);
		
		gl_texture2d(
			const config&);
        
    private:
        
		const config _cfg;
        unsigned _gl_handle;

		struct
		{
			source_type source = source_type::UNDEFINED;
			glm::ivec2 res = {};
			texture_format format = texture_format::UNDEFINED;
			buffer_view<char> mapped_view = {};
			std::unique_ptr<char[]> mapped;
		} _state;
        
        perf::metric _gpu_mem_metric;
        perf::metric _cpu_mem_metric;
    };
}
