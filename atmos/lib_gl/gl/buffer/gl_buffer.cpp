//
//#include "gl/buffer/gl_buffer.h"
//
//#include "gl/defines/gl_includes.h"
//
//#include "framework/utility/buffer_view.h"
//
// using namespace igpu;
//
// namespace
//{
//	GLenum gl_access( buffer_mapping mapping )
//	{
//		switch ( mapping )
//		{
//		case buffer_mapping::WRITE_COMBINE:
//			return GL_MAP_WRITE_BIT;
//		case buffer_mapping::PRESERVE:
//			return GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
//		}
//
//		LOG_CRITICAL( "unhandled mapping:%s", to_string( mapping ).data() );
//		return GL_FALSE;
//	}
//
//	GLuint gl_gen_buffer()
//	{
//		GLuint gl_handle = 0;
//		glGenBuffers( 1, &gl_handle );
//		return gl_handle;
//	}
//
//	std::string to_string( unsigned gl_target )
//	{
//		std::string str;
//		switch ( gl_target )
//		{
//		case GL_ARRAY_BUFFER:
//			return "vertex buffer";
//		case GL_COPY_READ_BUFFER:
//			return "copy read buffer";
//		case GL_COPY_WRITE_BUFFER:
//			return "copy write buffer";
//		case GL_ELEMENT_ARRAY_BUFFER:
//			return "index buffer";
//		case GL_PIXEL_PACK_BUFFER:
//			return "pixel pack buffer";
//		case GL_PIXEL_UNPACK_BUFFER:
//			return "pixel unpack buffer";
//		case GL_TRANSFORM_FEEDBACK_BUFFER:
//			return "transform feedback buffer";
//		case GL_UNIFORM_BUFFER:
//			return "compute buffer";
//		}
//
//		LOG_CRITICAL( "unhandled gl target %d", gl_target );
//		return "<unknown>";
//	}
//
//	void* map( GLenum gl_target, GLenum gl_access )
//	{
//#if defined( GL_OES_mapbuffer )
//		return glMapBufferOES( gl_target, gl_access );
//#else
//		return glMapBuffer( gl_target, gl_access );
//#endif
//	}
//
//	void unmap( GLenum gl_target )
//	{
//#if GL_OES_mapbuffer
//		glUnmapBufferOES( gl_target );
//#else
//		glUnmapBuffer( gl_target );
//#endif
//	}
//
//	struct handle_scope
//	{
//		GLenum target = 0;
//		GLint active_handle = 0;
//		handle_scope( GLenum target, GLint handle )
//		{
//			this->target = target;
//			glGetIntegerv( target, &active_handle );
//			glBindBuffer( target, handle );
//		}
//
//		~handle_scope()
//		{
//			glBindBuffer( target, active_handle );
//		}
//	};
//}
//
// gl_buffer::gl_buffer( buffer_mapping mapping, unsigned gl_target )
//	: _gpu_mem_metric( perf::category::GPU_MEM_USAGE, ::to_string( gl_target ) )
//	, _mapping( mapping )
//	, _gl_target( gl_target )
//	, _gl_handle( gl_gen_buffer() )
//	, _gl_usage( GL_DYNAMIC_DRAW )
//	, _gl_access( gl_access( mapping ) )
//{}
//
// gl_buffer::~gl_buffer()
//{
//	glDeleteBuffers( 1, &_gl_handle );
//}
//
// void gl_buffer::map( buffer_view_base* out_buffer_view )
//{
//	if ( _mapped )
//	{
//		LOG_CRITICAL( "map/unmap mismatch" );
//		return;
//	}
//
//	handle_scope scope( _gl_target, _gl_handle );
//
//	if ( _byte_capacity < out_buffer_view->byte_size() )
//	{
//		std::vector< char > temp;
//		if ( 0 != _byte_capacity && _mapping == buffer_mapping::PRESERVE )
//		{
//			char* temp_map = (char*)::map( _gl_target, _gl_access );
//			temp.assign( temp_map, temp_map + _byte_size );
//			::unmap( _gl_target );
//		}
//
//		_byte_capacity = out_buffer_view->byte_size();
//		glBufferData( _gl_target, _byte_capacity, temp.data(), _gl_usage );
//
//		_gpu_mem_metric.reset();
//		_gpu_mem_metric.add( _byte_capacity );
//	}
//
//	_mapped = ::map( _gl_target, _gl_access );
//
//	_byte_size = out_buffer_view->byte_size();
//	*out_buffer_view = buffer_view_base(
//		out_buffer_view->size(),
//		_mapped,
//		out_buffer_view->stride() );
//}
//
// void gl_buffer::unmap()
//{
//	if ( !_mapped )
//	{
//		LOG_CRITICAL( "map/unmap mismatch" );
//	}
//	else
//	{
//		handle_scope scope( _gl_target, _gl_handle );
//
//#if GL_OES_mapbuffer
//		glUnmapBufferOES( _gl_target );
//#else
//		glUnmapBuffer( _gl_target );
//#endif
//
//		_mapped = nullptr;
//	}
//}
//
// void gl_buffer::release()
//{
//	if ( _mapped )
//	{
//		LOG_CRITICAL( "cannot release while memory is mapped" );
//	}
//	else
//	{
//		handle_scope scope( _gl_target, _gl_handle );
//
//		_byte_capacity = 0;
//		glBufferData( _gl_target, 0, nullptr, _gl_usage );
//		_gpu_mem_metric.reset();
//	}
//}
//
// unsigned gl_buffer::gl_handle() const
//{
//	return _gl_handle;
//}
//
// size_t gl_buffer::byte_size() const
//{
//	return _byte_size;
//}
