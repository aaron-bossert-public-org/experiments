
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

	std::string to_string(unsigned gl_target)
	{
		std::string str;
		switch (gl_target)
		{
		case GL_ARRAY_BUFFER:
			return "vertex buffer";
		case GL_COPY_READ_BUFFER:
			return "copy read buffer";
		case GL_COPY_WRITE_BUFFER:
			return "copy write buffer";
		case GL_ELEMENT_ARRAY_BUFFER:
			return "index buffer";
		case GL_PIXEL_PACK_BUFFER:
			return "pixel pack buffer";
		case GL_PIXEL_UNPACK_BUFFER:
			return "pixel unpack buffer";
		case GL_TRANSFORM_FEEDBACK_BUFFER:
			return "transform feedback buffer";
		case GL_UNIFORM_BUFFER:
			return "compute buffer";
		}

		LOG_CRITICAL("unhandled gl target %d", gl_target);
		return "<unknown>";
	}
}

gl_buffer::gl_buffer(
	const config& cfg,
	unsigned gl_target)
	: _gpu_mem_metric(perf::category::GPU_MEM_USAGE, ::to_string(gl_target))
	, _cfg(cfg)
	, _gl_target(gl_target)
	, _gl_handle(gl_gen_buffer())
	, _gl_usage(gl_usage(cfg.usage))
	, _gl_access(gl_access())
{
}

gl_buffer::~gl_buffer()
{
	glDeleteBuffers(1, &_gl_handle);
}

const gl_buffer::config& gl_buffer::cfg() const
{
	return _cfg;
}

void gl_buffer::map(size_t byte_size, buffer_view_base* out_buffer_view)
{
	if (_mapped)
	{
		LOG_CRITICAL("map/unmap mismatch");
		unmap();
	}

	GLint active_handle = 0;
	glGetIntegerv(_gl_target, &active_handle);
	glBindBuffer(_gl_target, _gl_handle);

	if (_byte_capacity < byte_size)
	{
		_byte_capacity = byte_size;
		glBufferData(_gl_target, _byte_capacity, nullptr, _gl_usage);

		_gpu_mem_metric.reset();
		_gpu_mem_metric.add(_byte_capacity);
	}

#if defined(GL_OES_mapbuffer )
	_mapped =
		glMapBufferOES(_gl_target, _gl_access);
#else
	_mapped =
		glMapBuffer(_gl_target, _gl_access);
#endif

	glBindBuffer(_gl_target, active_handle);

	*out_buffer_view = buffer_view_base(
		byte_size / out_buffer_view->stride(),
		_mapped,
		out_buffer_view->stride());
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

		_byte_capacity = 0;
		glBufferData(_gl_target, 0, nullptr, _gl_usage);
		_gpu_mem_metric.reset();		
	}
}

unsigned gl_buffer::gl_handle() const
{
	return _gl_handle;
}

size_t gl_buffer::byte_capacity() const
{
	return _byte_capacity;
}

