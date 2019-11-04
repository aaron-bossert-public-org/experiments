
#pragma once

#include "gl/texture/gl_depth_target.h"

#include "igpu/texture/depth_target.h"

#include "framework/perf/metrics.h"

namespace igpu
{
	class gl_depth_buffer
		: public depth_target
		, gl_depth_target
	{
	public:
		const config& cfg() const override;

		void attach() const override;

		void gl_handle( int gl_handle );

		static std::unique_ptr< gl_depth_buffer > make_empty();

		static std::unique_ptr< gl_depth_buffer > make( const config& );

		~gl_depth_buffer();

	protected:
		gl_depth_buffer( const config&, int gl_handle );

	private:
		const config _cfg;
		int _gl_handle = 0;
		perf::metric _gpu_mem_metric;
	};
}
