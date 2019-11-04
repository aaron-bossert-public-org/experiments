
#pragma once


#include "igpu/buffer/buffer_raw.h"
#include "igpu/texture/texture2d.h"
#include "framework/perf/metrics.h"
#include "glm/vec2.hpp"

namespace igpu
{
	class gl_texture2d : public texture2d
	{
	public:


		unsigned gl_handle() const;

		~gl_texture2d() override;

		void map(size_t, buffer_view_base*) override;

		void unmap() override;

		void unmap(const state&) override;

		size_t byte_capacity() const override;

		const config& cfg() const override;

		const state& current_state() const override;

		static std::unique_ptr<gl_texture2d> make(const config&);

	private:

		void unmap(
			const buffer_view<char>&,
			const state&);

		gl_texture2d(
			const config&);
        
    private:
        
		const config _cfg;
		buffer_raw _buffer;
		unsigned _gl_handle;

		state _state;
        
        perf::metric _gpu_mem_metric;
    };
}
