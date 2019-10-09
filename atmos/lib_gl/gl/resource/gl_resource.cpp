
#include <gl/resource/gl_resource.h>

// GL implementation includes - begin
#include <gl/defines/gl_includes.h>
// GL implementation includes - end

#include <framework/utility/buffer_view.h>

using namespace igpu;

namespace
{
	GLenum gl_usage(resource_usage usage)
	{
		switch (usage)
		{
		case resource_usage::STATIC:
			return GL_STATIC_DRAW;
		case resource_usage::DYNAMIC:
			return GL_DYNAMIC_DRAW;
		}

		LOG_CONTEXT(CRITICAL, "unhandled usage:%s", to_string(usage).data());
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

gl_resource::gl_resource(
	const std::string_view& metric,
	resource_usage usage,
	unsigned gl_target)
	: _gpu_mem_metric(perf::category::GPU_MEM_USAGE, metric)
	, _usage(usage)
	, _gl_target(gl_target)
	, _gl_handle(gl_gen_buffer())
	, _gl_usage(gl_usage(usage))
	, _gl_access(gl_access())
{
}

gl_resource::~gl_resource()
{
	glDeleteBuffers(1, &_gl_handle);
}

void gl_resource::map(buffer_view_base& resource_view, size_t count)
{
	if (_mapped)
	{
		LOG_CONTEXT(CRITICAL, "map/unmap mismatch");
	}
	else
	{
		GLint active_handle = 0;
		glGetIntegerv(_gl_target, &active_handle);
		glBindBuffer(_gl_target, _gl_handle);

		size_t required_capacity = resource_view.stride() * count;
		if (_capacity < required_capacity)
		{
			_capacity = required_capacity;
			glBufferData(_gl_target, _capacity, nullptr, _gl_usage);

			_gpu_mem_metric.reset();
			_gpu_mem_metric.add(float(_capacity));
		}

		_mapped =
#if defined(GL_EXT_map_buffer_range )
			glMapBufferRangeEXT(_gl_target, 0, count, _gl_access);
#else
			glMapBufferRange(_gl_target, 0, count, _gl_access);
#endif
		
		glBindBuffer(_gl_target, active_handle);

		resource_view = buffer_view_base(
			count,
			_mapped,
			resource_view.stride());
	}
}

void gl_resource::map_subrange(buffer_view_base& resource_view, size_t offset, size_t count)
{
	if (_mapped)
	{
		LOG_CONTEXT(CRITICAL, "map/unmap mismatch");
	}
	else if (_capacity < resource_view.stride()*(offset + count))
	{
		LOG_CONTEXT(
			CRITICAL,
			"request requires %d bytes but only %d bytes is mapped",
			resource_view.stride()*(offset + count),
			_capacity);
	}
	else
	{
		GLint active_handle = 0;
		glGetIntegerv(_gl_target, &active_handle);
		glBindBuffer(_gl_target, _gl_handle);

		_mapped =
#if defined(GL_EXT_map_buffer_range )
			glMapBufferRangeEXT(_gl_target, offset, count, _gl_access);
#else
			glMapBufferRange(_gl_target, offset, count, _gl_access);
#endif

		glBindBuffer(_gl_target, active_handle);

		resource_view = buffer_view_base(
			count,
			_mapped,
			resource_view.stride());
	}
}

void gl_resource::unmap(buffer_view_base& resource_view)
{
	if (resource_view.data() != _mapped)
	{
		LOG_CONTEXT(WARNING,
			"buffer to unmap:%p does not match internal record of mapped address:%p",
			resource_view.data(),
			_mapped);
	}
	else
	{
		unmap();

		resource_view = buffer_view_base(
			resource_view.size(),
			nullptr,
			resource_view.stride());
	}
}

void gl_resource::unmap()
{
	if (!_mapped)
	{
		LOG_CONTEXT(CRITICAL, "map/unmap mismatch");
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

void gl_resource::release()
{
	if (_mapped)
	{
		LOG_CONTEXT(CRITICAL, "cannot release while memory is mapped");
	}
	else
	{
		GLint active_handle = 0;
		glGetIntegerv(_gl_target, &active_handle);
		glBindBuffer(_gl_target, _gl_handle);

		_capacity = 0;
		glBufferData(_gl_target, _capacity, nullptr, _gl_usage);
		_gpu_mem_metric.reset();		
	}
}

unsigned gl_resource::gl_handle() const
{
	return _gl_handle;
}

size_t gl_resource::capacity() const
{
	return _capacity;
}

resource_usage gl_resource::usage() const
{
	return _usage;
}


