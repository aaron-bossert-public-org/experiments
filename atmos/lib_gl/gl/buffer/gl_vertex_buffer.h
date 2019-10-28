
#pragma once

#include <framework/perf/metrics.h>
#include <igpu/buffer/vertex_buffer.h>

namespace igpu
{

	class gl_vertex_buffer : public vertex_buffer
	{
	public:

		virtual unsigned gl_handle() const = 0;

		static std::unique_ptr<gl_vertex_buffer> make(const config&);

	protected:

		gl_vertex_buffer() = default;
	};
}
