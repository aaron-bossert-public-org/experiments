
#include <gl/resource/gl_geometry.h>

#include <framework/logging/log.h>

// GL implementation includes - begin
#include <gl/defines/gl_includes.h>
#include <gl/resource/gl_index_resource.h>
#include <gl/resource/gl_vertex_resource.h>
// GL implementation includes - end

#include <igpu/resource/vertex_format.h>


using namespace igpu;

GLenum gl_geometry::gl_topology() const
{
	return _gl_topology;
}

GLuint gl_geometry::vao() const
{
    return _vao;
}

std::unique_ptr<gl_geometry> gl_geometry::make(
	std::string name,
	igpu::topology topology,
	size_t element_start,
	size_t element_count,
	std::shared_ptr<igpu::index_resource> index_resource,
	std::vector<std::shared_ptr<vertex_resource>> vertex_resources)
{       
	if (nullptr == index_resource)
	{
		LOG_CONTEXT(CRITICAL,
			"index resource is null");
		return nullptr;
	}

	for (size_t i = vertex_resources.size(); i-- > 0;)
	{
		const auto& vertex_resource = vertex_resources[i];
		if (nullptr == vertex_resource)
		{
			LOG_CONTEXT(CRITICAL,
				"vertex resource is null vertex_resources[%d]",
				(int)i);

			vertex_resources[i] = vertex_resources.back();
			vertex_resources.pop_back();
		}
	}

	GLenum gl_topology = GL_FALSE;
	switch (topology)
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
		LOG_CONTEXT(CRITICAL, "unhandled topology: %s", to_string(topology).data());
		return nullptr;
	}
	
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);

	if (vao)
	{
		glBindVertexArray(vao);

		for (const auto& vertex_resource : vertex_resources)
		{
			const gl_vertex_resource* gl_v_resource = (gl_vertex_resource*)vertex_resource.get();
			glBindBuffer(GL_ARRAY_BUFFER, gl_v_resource->gl_handle());

			for (const gl_vertex_attribute& attribute : gl_v_resource->gl_format().attributes())
			{
				glEnableVertexAttribArray(attribute.location);
				glVertexAttribPointer(
					attribute.location,
					attribute.size,
					attribute.type, 
					GL_FALSE/*normalized*/, 
					gl_v_resource->cfg().format.stride,
					attribute.offset + (char*)nullptr);
			}
		}

		gl_index_resource* gl_i_resource = (gl_index_resource*)index_resource.get();
		if (gl_i_resource)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_i_resource->gl_handle());
		}

		glBindVertexArray(0);
	}

    return std::unique_ptr<gl_geometry>(
		new gl_geometry(
			std::move(name),
			topology,
			element_start,
			element_count,
			gl_topology,
			vao,
			std::move(index_resource),
			std::move(vertex_resources)));
}

gl_geometry::~gl_geometry()
{
    glDeleteVertexArrays(1, &_vao);
}

gl_geometry::gl_geometry(
	std::string name,
	igpu::topology topology,
	size_t element_start,
	size_t element_count,
	unsigned gl_topology,
	unsigned vao,
	std::shared_ptr<igpu::index_resource> index_resource,
	std::vector<std::shared_ptr<vertex_resource>> vertex_resources)
: geometry(
	std::move(name),
	topology,
	element_start,
	element_count,
	std::move(index_resource),
	std::move(vertex_resources))
, _gl_topology(gl_topology)
, _vao(vao)
{
}
