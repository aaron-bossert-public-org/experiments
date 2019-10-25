
#include <gl/material/gl_fragment_shader.h>
#include <gl/material/gl_shader.h>

using namespace igpu;


std::unique_ptr<gl_fragment_shader> gl_fragment_shader::make()
{
	return gl_shader_t<gl_fragment_shader>::make(
		GL_FRAGMENT_SHADER);
}
