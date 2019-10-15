
#include <gl/buffer/gl_vertex_buffer.h>

// GL implementation includes - begin
#include <gl/defines/gl_includes.h>
// GL implementation includes - end

using namespace igpu;

const gl_vertex_format& gl_vertex_buffer::gl_format() const
{
	return _format;
}

std::unique_ptr<gl_vertex_buffer> gl_vertex_buffer::make(const config& cfg, const vertex_constraints& constraints)
{
	if (!is_valid(cfg.usage))
	{
		LOG_CONTEXT(CRITICAL, "invalid usage:%d", (int)cfg.usage);
	}
	else
	{
		return std::unique_ptr<gl_vertex_buffer>(
			new gl_vertex_buffer(
				cfg,
				constraints));
	}

	return nullptr;
}

gl_vertex_buffer::gl_vertex_buffer(const config& cfg, const vertex_constraints& constraints)
	: _format(constraints, cfg.format)
	, gl_buffer_t (
		cfg,
		"GPU Vertex Mem",
		cfg.usage,
		GL_ARRAY_BUFFER)
{}
