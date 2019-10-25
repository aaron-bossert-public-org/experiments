//
//#include <gl/material/gl_vertex_shader.h>
//#include <gl/material/gl_shader.h>
//#include <gl/material/gl_compiler.h>
//
//using namespace igpu;
//
//namespace
//{
//	struct
//	{
//		std::unique_ptr<char[]> raw;
//		buffer_view_base view;
//	} _mapped;
//}
//
//
//gl_shader::gl_shader(GLenum type)
//: _type(type)
//{
//}
//
//gl_shader::~gl_shader()
//{
//	if (_gl_handle)
//	{
//		glDeleteProgram(_gl_handle);
//	}
//}
//
//GLuint gl_shader::gl_handle() const
//{
//	return _gl_handle;
//}
//
//void gl_shader::map(
//	size_t byte_size, 
//	buffer_view_base* out_buffer_view)
//{
//	if (_mapped.raw)
//	{
//		LOG_CRITICAL("map/unmap mismatch");
//	}
//	else if(_gl_handle)
//	{
//		LOG_CRITICAL("cannot map shader multiple times");
//	}
//	else
//	{
//		_mapped.raw.reset(new char[byte_size]);
//		_mapped.view = 
//			*out_buffer_view = buffer_view_base(
//			byte_size,
//			_mapped.raw.get(),
//			out_buffer_view->stride());
//	}
//}
//
//void gl_shader::unmap()
//{
//	if (!_mapped.raw)
//	{
//		LOG_CRITICAL("map/unmap mismatch");
//	}
//	else if (_gl_handle)
//	{
//		LOG_CRITICAL("cannot map shader multiple times");
//	}
//	else
//	{
//		_gl_handle = gl_compile_shader_code(_type, _mapped.view);
//
//		_mapped.raw.reset();
//		_mapped.view.reset();
//	}
//}
