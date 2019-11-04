
#include "gl/buffer/gl_vertex_buffer.h"

#include "gl/buffer/gl_buffer.h"
#include "gl/defines/gl_includes.h"

using namespace igpu;

std::unique_ptr<gl_vertex_buffer> gl_vertex_buffer::make(const config& cfg)
{
	return gl_buffer_t<gl_vertex_buffer>::make(
		cfg,
		GL_ARRAY_BUFFER);
}

