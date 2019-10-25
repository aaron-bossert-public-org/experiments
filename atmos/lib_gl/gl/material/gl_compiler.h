
#pragma once

class buffer_view_base;

namespace igpu
{
	class gl_context;
	class vertex_constraints;
	class shaders;

	unsigned gl_compile_shader_code(
		unsigned type, 
		const buffer_view_base& source_code);

    unsigned gl_compile(
		const gl_context*,
		const shaders& shaders);
}
