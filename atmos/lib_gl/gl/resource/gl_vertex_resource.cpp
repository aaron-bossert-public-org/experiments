
#include <gl/resource/gl_vertex_resource.h>

// GL implementation includes - begin
#include <gl/defines/gl_includes.h>
// GL implementation includes - end

using namespace igpu;

const vertex_resource::config& gl_vertex_resource::cfg() const
{
	return _cfg;
}

void gl_vertex_resource::map(buffer_view_base& resource_view, size_t count)
{
	_gl_resource.map(resource_view, count);
}

void gl_vertex_resource::map_subrange(buffer_view_base& resource_view, size_t offset, size_t count)
{
	_gl_resource.map_subrange(resource_view, offset, count);
}

void gl_vertex_resource::unmap(buffer_view_base& resource_view)
{
	_gl_resource.unmap(resource_view);
}

size_t gl_vertex_resource::capacity() const
{
	return _gl_resource.capacity();
}

const gl_vertex_format& gl_vertex_resource::gl_format() const
{
	return _format;
}
unsigned gl_vertex_resource::gl_handle() const
{
	return _gl_resource.gl_handle();
}

std::unique_ptr<gl_vertex_resource> gl_vertex_resource::make(const config& cfg, const vertex_constraints& constraints)
{
	if (!is_valid(cfg.usage))
	{
		LOG_CONTEXT(CRITICAL, "invalid usage:%d", (int)cfg.usage);
	}
	else
	{
		return std::unique_ptr<gl_vertex_resource>(
			new gl_vertex_resource(
				cfg,
				constraints));
	}

	return nullptr;
}

gl_vertex_resource::gl_vertex_resource(const config& cfg, const vertex_constraints& constraints)
	: _format(constraints, cfg.format)
	, _cfg(cfg)
	, _gl_resource(
		"GPU Vertex Mem",
		cfg.usage,
		GL_ARRAY_BUFFER)
{}
