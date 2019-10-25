
#include <gl/buffer/gl_vertex_buffer.h>

#include <gl/buffer/gl_buffer.h>
#include <gl/defines/gl_includes.h>

using namespace igpu;

const gl_vertex_format& gl_vertex_buffer::gl_format() const
{
	return _format;
}

std::unique_ptr<gl_vertex_buffer> gl_vertex_buffer::make(const config& cfg, const vertex_constraints& constraints)
{
	return gl_buffer_t<gl_vertex_buffer>::make(
		cfg,
		GL_ARRAY_BUFFER,
		cfg,
		constraints);
}

gl_vertex_buffer::gl_vertex_buffer(const config& cfg, const vertex_constraints& constraints)
	: _format(constraints, cfg.format)
{}
