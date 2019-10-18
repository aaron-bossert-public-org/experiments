

#include <gl/buffer/gl_index_buffer.h>

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

		LOG_CRITICAL("unhandled index_format value: %s", to_string(format).data());

		return GL_FALSE;
	}
}

unsigned gl_index_buffer::gl_format() const
{
	return _gl_format;
}

std::unique_ptr<gl_index_buffer> gl_index_buffer::make(const config& cfg)
{
	unsigned gl_format = to_gl_format(cfg.format);

	if (!is_valid(cfg.format))
	{
		LOG_CRITICAL("invalid format:%d", (int)cfg.format);
	}
	else if (!is_valid(cfg.usage))
	{
		LOG_CRITICAL("invalid usage:%d", (int)cfg.usage);
	}
	else if (GL_FALSE == gl_format)
	{
		LOG_CRITICAL("unhandled format:%s", to_string(cfg.format).data());
	}
	else
	{
		return std::unique_ptr<gl_index_buffer>(
			new gl_index_buffer(
				cfg,
				gl_format));
	}

	return nullptr;
}

gl_index_buffer::gl_index_buffer(const config& cfg, unsigned gl_format)
: gl_buffer_t (
	cfg,
	"GPU Index Mem",
	cfg.usage,
	GL_ELEMENT_ARRAY_BUFFER) 
, _gl_format(gl_format)
{
}