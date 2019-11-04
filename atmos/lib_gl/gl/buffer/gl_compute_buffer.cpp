
#include "gl/buffer/gl_compute_buffer.h"

#include "gl/buffer/gl_buffer.h"
#include "gl/defines/gl_includes.h"

using namespace igpu;

std::unique_ptr< gl_compute_buffer > gl_compute_buffer::make(
	const config& cfg )
{
	return gl_buffer_t< gl_compute_buffer >::make( cfg, GL_UNIFORM_BUFFER );
}
