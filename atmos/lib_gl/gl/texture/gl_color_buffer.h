
#pragma once

#include "gl/texture/gl_render_target.h"

#include "igpu/texture/render_target.h"

#include "framework/perf/metrics.h"

namespace igpu
{
	class gl_render_buffer
		: public render_target
		, gl_render_target
	{
	public:
		const config& cfg() const override;

		void attach() const override;

		void gl_handle( int );

		static std::unique_ptr< gl_render_buffer > make_empty();

		static std::unique_ptr< gl_render_buffer > make( const config& );

		~gl_render_buffer();

	protected:
		gl_render_buffer( const config&, int gl_handle );

	private:
		const config _cfg;
		int _gl_handle = 0;
		perf::metric _gpu_mem_metric;
	};
}
