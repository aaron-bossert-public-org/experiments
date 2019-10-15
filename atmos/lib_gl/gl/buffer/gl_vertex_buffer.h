
#pragma once

#include <framework/perf/metrics.h>
#include <igpu/buffer/vertex_buffer.h>
#include <gl/buffer/gl_buffer.h>
#include <gl/buffer/gl_vertex_format.h>

namespace igpu
{
	class gl_vertex_buffer : public gl_buffer_t < vertex_buffer >
	{
	public:

		const gl_vertex_format& gl_format() const;

		static std::unique_ptr<gl_vertex_buffer> make(const config&, const vertex_constraints&);

	protected:

		gl_vertex_buffer(const config&, const vertex_constraints&);

	private:

		const gl_vertex_format _format;
	};
}
