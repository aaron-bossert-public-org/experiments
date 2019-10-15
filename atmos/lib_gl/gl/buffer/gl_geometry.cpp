
#include <gl/buffer/gl_geometry.h>

#include <framework/logging/log.h>

// GL implementation includes - begin
#include <gl/buffer/gl_index_buffer.h>
#include <gl/buffer/gl_vertex_buffer.h>
#include <gl/defines/gl_includes.h>
// GL implementation includes - end

#include <igpu/buffer/vertex_format.h>

using namespace igpu;

const gl_geometry::config& gl_geometry::cfg() const
{
	return _cfg;
}

size_t gl_geometry::element_start() const
{
	return _element_start;
}

void gl_geometry::element_start(size_t element_start)
{
	_element_start = element_start;
}

size_t gl_geometry::element_count() const
{
	return _element_count;
}

void gl_geometry::element_count(size_t element_count)
{
	_element_count = element_count;
}

GLenum gl_geometry::gl_topology() const
{
	return _gl_topology;
}

GLuint gl_geometry::vao() const
{
    return _vao;
}

std::unique_ptr<gl_geometry> gl_geometry::make(
	const config& cfg)
{
	if (nullptr == cfg.index_buffer)
	{
		LOG_CONTEXT(CRITICAL,
			"index buffer is null");
		return nullptr;
	}

	for (size_t i = 0; i < cfg.vertex_buffers.size(); ++i)
	{
		const auto& vertex_buffer = cfg.vertex_buffers[i];
		if (nullptr == vertex_buffer)
		{
			LOG_CONTEXT(CRITICAL,
				"vertex buffer is null vertex_buffers[%d]",
				(int)i);

			return nullptr;
		}
	}

	GLenum gl_topology = GL_FALSE;
	switch (cfg.topology)
	{
	case topology::TRIANGLE_LIST:
		gl_topology = GL_TRIANGLES;
		break;
	case topology::TRIANGLE_STRIP:
		gl_topology = GL_TRIANGLE_STRIP;
		break;
	case topology::TRIANGLE_FAN:
		gl_topology = GL_TRIANGLE_FAN;
		break;
	case topology::LINE_LIST:
		gl_topology = GL_LINES;
		break;
	case topology::LINE_STRIP:
		gl_topology = GL_LINE_STRIP;
		break;
	case topology::POINT_LIST:
		gl_topology = GL_POINTS;
		break;
	default:
		LOG_CONTEXT(CRITICAL, "unhandled topology: %s", to_string(cfg.topology).data());
		return nullptr;
	}
	
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);

	if (vao)
	{
		glBindVertexArray(vao);

		for (const auto& vertex_buffer : cfg.vertex_buffers)
		{
			const gl_vertex_buffer* gl_v_buffer = (gl_vertex_buffer*)vertex_buffer.get();
			glBindBuffer(GL_ARRAY_BUFFER, gl_v_buffer->gl_handle());

			for (const gl_vertex_attribute& attribute : gl_v_buffer->gl_format().attributes())
			{
				glEnableVertexAttribArray(attribute.location);
				glVertexAttribPointer(
					attribute.location,
					attribute.size,
					attribute.type, 
					GL_FALSE/*normalized*/, 
					gl_v_buffer->cfg().format.stride,
					attribute.offset + (char*)nullptr);
			}
		}

		gl_index_buffer* gl_i_buffer = (gl_index_buffer*)cfg.index_buffer.get();
		if (gl_i_buffer)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_i_buffer->gl_handle());
		}

		glBindVertexArray(0);
	}

    return std::unique_ptr<gl_geometry>(
		new gl_geometry(
			cfg,
			gl_topology,
			vao));
}

gl_geometry::~gl_geometry()
{
    glDeleteVertexArrays(1, &_vao);
}

gl_geometry::gl_geometry(
	const config& cfg,
	unsigned gl_topology,
	unsigned vao)
: _cfg(cfg)
, _element_start(0)
, _element_count(cfg.index_buffer->count())
, _gl_topology(gl_topology)
, _vao(vao)
{
}
