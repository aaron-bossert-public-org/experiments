
#include <gl/buffer/gl_buffer.h>

// GL implementation includes - begin
#include <gl/defines/gl_includes.h>
// GL implementation includes - end

#include <framework/utility/buffer_view.h>

using namespace igpu;

namespace
{
	GLenum gl_usage(buffer_usage usage)
	{
		switch (usage)
		{
		case buffer_usage::STATIC:
			return GL_STATIC_DRAW;
		case buffer_usage::DYNAMIC:
			return GL_DYNAMIC_DRAW;
		}

		LOG_CRITICAL("unhandled usage:%s", to_string(usage).data());
		return GL_FALSE;
	}

	GLenum gl_access()
	{
		// acces as write only, revisit if other usages are added
		return  
			GL_MAP_WRITE_BIT | GL_MAP_READ_BIT
#if defined(GL_MAP_INVALIDATE_RANGE_BIT )
			| GL_MAP_INVALIDATE_RANGE_BIT
#elif defined(GL_MAP_INVALIDATE_RANGE_BIT_EXT)
			| GL_MAP_INVALIDATE_RANGE_BIT_EXT
#endif
#if defined(GL_MAP_UNSYNCHRONIZED_BIT )
			| GL_MAP_UNSYNCHRONIZED_BIT
#elif defined(GL_MAP_UNSYNCHRONIZED_BIT_EXT)
			| GL_MAP_UNSYNCHRONIZED_BIT_EXT
#endif
			;
	}

	GLuint  gl_gen_buffer()
	{
		GLuint gl_handle = 0;
		glGenBuffers(1, &gl_handle);
		return gl_handle;
	}
}

gl_buffer::gl_buffer(
	const std::string_view& metric,
	buffer_usage usage,
	unsigned gl_target)
	: _gpu_mem_metric(perf::category::GPU_MEM_USAGE, metric)
	, _usage(usage)
	, _gl_target(gl_target)
	, _gl_handle(gl_gen_buffer())
	, _gl_usage(gl_usage(usage))
	, _gl_access(gl_access())
{
}

gl_buffer::~gl_buffer()
{
	glDeleteBuffers(1, &_gl_handle);
}

void gl_buffer::map(buffer_view_base& buffer_view, size_t byte_size)
{
	if (_mapped)
	{
		LOG_CRITICAL("map/unmap mismatch");
		unmap();
	}

	GLint active_handle = 0;
	glGetIntegerv(_gl_target, &active_handle);
	glBindBuffer(_gl_target, _gl_handle);

	if (_byte_size < byte_size)
	{
		_byte_size = byte_size;
		glBufferData(_gl_target, _byte_size, nullptr, _gl_usage);

		_gpu_mem_metric.reset();
		_gpu_mem_metric.add(_byte_size);
	}

//	if (0 <= offset)
//	{
//		_mapped =
//#if defined(GL_EXT_map_buffer_range )
//			glMapBufferRangeEXT(_gl_target, offset, size, _gl_access);
//#else
//			glMapBufferRange(_gl_target, offset, count, _gl_access);
//#endif
//	}
//	else
	{
		_mapped =
#if defined(GL_OES_mapbuffer )
			glMapBufferOES(_gl_target, _gl_access);
#else
			glMapBuffer(_gl_target, _gl_access);
#endif

	}

	glBindBuffer(_gl_target, active_handle);

	buffer_view = buffer_view_base(
		byte_size / buffer_view.stride(),
		_mapped,
		buffer_view.stride());
}

void gl_buffer::unmap()
{
	if (!_mapped)
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else
	{
		GLint active_handle = 0;
		glGetIntegerv(_gl_target, &active_handle);
		glBindBuffer(_gl_target, _gl_handle);

#if GL_OES_mapbuffer
		glUnmapBufferOES(_gl_target);
#else
		glUnmapBuffer(_gl_target);
#endif
		glBindBuffer(_gl_target, active_handle);

		_mapped = nullptr;
	}
}

void gl_buffer::release()
{
	if (_mapped)
	{
		LOG_CRITICAL("cannot release while memory is mapped");
	}
	else
	{
		GLint active_handle = 0;
		glGetIntegerv(_gl_target, &active_handle);
		glBindBuffer(_gl_target, _gl_handle);

		_byte_size = 0;
		glBufferData(_gl_target, 0, nullptr, _gl_usage);
		_gpu_mem_metric.reset();		
	}
}

unsigned gl_buffer::gl_handle() const
{
	return _gl_handle;
}

size_t gl_buffer::byte_size() const
{
	return _byte_size;
}

buffer_usage gl_buffer::usage() const
{
	return _usage;
}


