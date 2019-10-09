
#include <gl/resource/gl_compute_resource.h>

// GL implementation includes - begin
#include <gl/defines/gl_includes.h>
// GL implementation includes - end

using namespace igpu;

const compute_resource::config& gl_compute_resource::cfg() const
{
	return _cfg;
}

void gl_compute_resource::map(buffer_view_base& mapped, size_t count)
{
	_gl_resource.map(mapped, count);
}

void gl_compute_resource::map_subrange(buffer_view_base& mapped, size_t offset, size_t count)
{
	_gl_resource.map_subrange(mapped, offset, count);
}

void gl_compute_resource::unmap(buffer_view_base& mapped)
{
	_gl_resource.unmap(mapped);
}

size_t gl_compute_resource::capacity() const
{
	return _gl_resource.capacity();
}

unsigned gl_compute_resource::gl_handle() const
{
	return _gl_resource.gl_handle();
}

std::unique_ptr<gl_compute_resource> gl_compute_resource::make(const config& cfg)
{
	if (!is_valid(cfg.usage))
	{
		LOG_CONTEXT(CRITICAL, "invalid usage:%d", (int)cfg.usage);
	}
	else
	{
		return std::unique_ptr<gl_compute_resource>(new gl_compute_resource(cfg));
	}

	return nullptr;
}

gl_compute_resource::gl_compute_resource(const config& cfg)
	: _gl_resource(
		"GPU Compute Mem",
		cfg.usage,
		GL_UNIFORM_BUFFER)
	, _cfg(cfg)
{}