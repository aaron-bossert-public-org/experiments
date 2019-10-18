
#include <gl/buffer/gl_vertex_format.h>

// GL implementation includes - begin
#include <gl/defines/gl_includes.h>
// GL implementation includes - end

#include <igpu/context/vertex_constraints.h>

using namespace igpu;

const std::vector<gl_vertex_attribute>& gl_vertex_format::attributes() const
{
	return _attributes;
}

gl_vertex_format::gl_vertex_format(const vertex_constraints& constraints, const config& cfg)
	: vertex_format(cfg)
{
	_attributes.reserve(cfg.attributes.size());

	for (const auto& attribute : cfg.attributes)
	{
		int size = 4;
		unsigned type = GL_FLOAT;
		GLuint location = (GLuint)constraints.find_location(attribute.parameter.name);
		const vertex_parameter ref_parameter = constraints.find_parameter(attribute.parameter.name);

		ASSERT_CONTEXT(
			attribute.parameter.components == ref_parameter.components,
			"attribute:%s components:%s does not match reference contained in vertex_constraints:%s",
			attribute.parameter.name.c_str(),
			to_string(attribute.parameter.components).data(),
			to_string(ref_parameter.components).data());

		switch (attribute.parameter.components)
		{
		case components::FLOAT1:
			size = 1;
			type = GL_FLOAT;
			break;
		case components::FLOAT2:
			size = 2;
			type = GL_FLOAT;
			break;
		case components::FLOAT3:
			size = 3;
			type = GL_FLOAT;
			break;
		case components::FLOAT4:
			size = 4;
			type = GL_FLOAT;
			break;
		default:
			LOG_CRITICAL(
				"unhandled format: %s",
				to_string(attribute.parameter.components).data());
		};

		_attributes.push_back({
			attribute,
			location,
			size,
			type
		});
	}
}
