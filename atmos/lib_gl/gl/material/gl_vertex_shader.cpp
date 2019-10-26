
#include <gl/material/gl_vertex_shader.h>
#include <gl/material/gl_shader.h>

using namespace igpu;


std::unique_ptr<gl_vertex_shader> gl_vertex_shader::make(const config& cfg)
{
	return gl_shader_t<gl_vertex_shader>::make(
		cfg,
		GL_VERTEX_SHADER);
}
