
#include "gl/material/gl_shader.h"

#include "gl/material/gl_compiler.h"
#include "gl/material/gl_vertex_shader.h"

using namespace igpu;

gl_shader::gl_shader( const config& cfg )
	: _cfg( cfg )
	, _buffer( {
		  cfg.usage,
	  } )
	, _gl_shader( glCreateShader( cfg.type ) )
{}

gl_shader::~gl_shader()
{
	if ( _gl_shader )
	{
		glDeleteShader( _gl_shader );
	}
}

GLuint gl_shader::gl_handle() const
{
	return _gl_shader;
}

void gl_shader::map( size_t byte_size, buffer_view_base* out_buffer_view )
{
	_buffer.map( byte_size, out_buffer_view );
}

void gl_shader::unmap()
{
	auto mapped_view = _buffer.mapped_view();

	if ( mapped_view.data() )
	{
		gl_compile_shader_code( _gl_shader, mapped_view );
	}

	_buffer.unmap();
}

size_t gl_shader::byte_capacity() const
{
	return _buffer.byte_capacity();
}
