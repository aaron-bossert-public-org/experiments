
#pragma once

#include <igpu/resource/vertex_format.h>

namespace igpu
{
	class vertex_constraints;

	struct gl_vertex_attribute : vertex_attribute
	{
		unsigned location = 0;
		int size = 0;
		unsigned type = 0;
	};


	class gl_vertex_format : public vertex_format 
	{
	public:

		const std::vector<gl_vertex_attribute>& attributes() const;

		gl_vertex_format(const vertex_constraints&, const config&);

	private:

		std::vector<gl_vertex_attribute> _attributes;
	};
}
