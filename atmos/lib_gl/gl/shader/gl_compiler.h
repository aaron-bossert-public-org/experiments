
#pragma once

#include "gl/material/gl_program.h"

class buffer_view_base;

namespace igpu
{
	class vertex_constraints;

	unsigned gl_compile_shader_code(
		unsigned shader,
		const buffer_view_base& source_code );

	unsigned gl_compile( const gl_program::config& cfg );
}
