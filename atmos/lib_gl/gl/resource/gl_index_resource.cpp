

#include <gl/resource/gl_index_resource.h>

// GL implementation includes - begin
#include <gl/defines/gl_includes.h>
// GL implementation includes - end

using namespace igpu;

namespace
{
	unsigned to_gl_format(index_format format)
	{
		switch (format)
		{
		case index_format::UNSIGNED_SHORT:
			return GL_UNSIGNED_SHORT;
		case index_format::UNSIGNED_INT:
			return GL_UNSIGNED_INT;
		};

		LOG_CONTEXT(CRITICAL, "unhandled index_format value: %s", to_string(format).data());

		return GL_FALSE;
	}
}

const index_resource::config& gl_index_resource::cfg() const
{
	return _cfg;
}

void gl_index_resource::map(buffer_view_base& mapped, size_t count)
{
	_gl_resource.map(mapped, count);
}

void gl_index_resource::map_subrange(buffer_view_base& mapped, size_t offset, size_t count)
{
	_gl_resource.map_subrange(mapped, offset, count);
}

void gl_index_resource::unmap(buffer_view_base& mapped)
{
	_gl_resource.unmap(mapped);
}

size_t gl_index_resource::capacity() const
{
	return _gl_resource.capacity();
}

unsigned gl_index_resource::gl_format() const
{
	return _gl_format;
}

unsigned gl_index_resource::gl_handle() const
{
	return _gl_resource.gl_handle();
}

std::unique_ptr<gl_index_resource> gl_index_resource::make(const config& cfg)
{
	unsigned gl_format = to_gl_format(cfg.format);

	if (!is_valid(cfg.format))
	{
		LOG_CONTEXT(CRITICAL, "invalid format:%d", (int)cfg.format);
	}
	else if (!is_valid(cfg.usage))
	{
		LOG_CONTEXT(CRITICAL, "invalid usage:%d", (int)cfg.usage);
	}
	else if (GL_FALSE == gl_format)
	{
		LOG_CONTEXT(CRITICAL, "unhandled format:%s", to_string(cfg.format).data());
	}
	else
	{
		return std::unique_ptr<gl_index_resource>(
			new gl_index_resource(
				cfg,
				gl_format));
	}

	return nullptr;
}

gl_index_resource::gl_index_resource(const config& cfg, unsigned gl_format)
: _gl_resource(
	"GPU Index Mem",
	cfg.usage,
	GL_ELEMENT_ARRAY_BUFFER) 
, _cfg(cfg)
, _gl_format(gl_format)
{
}