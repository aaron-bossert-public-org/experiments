
#pragma once

#include <igpu/buffer/vertex_parameter.h>
#include <string>
#include <vector>

#define IGPU_VERT_FORMAT_OF(V, ...)\
		IGPU_VERT_FORMAT_OF_(V, __VA_ARGS__)

// example usage of IGPU_VERT_FORMAT_OF:

//struct Vertex
//{
//	glm::vec3 pos;
//	glm::vec3 col;
//	glm::vec2 uv0;
//
//	bool operator==(const Vertex& other) const
//	{
//		return pos == other.pos && col == other.col && uv0 == other.uv0;
//	}
//};
//
//vertex_format format = IGPU_VERT_FORMAT_OF(Vertex, pos, col, uv0);


namespace igpu
{   
	struct vertex_attribute
	{
		vertex_parameter parameter;
		uint32_t offset = 0;
	};
	
	class vertex_format
	{
	public:

		struct config
		{
			uint32_t stride = 0;
			std::vector<vertex_attribute> attributes;
		};

		const config& cfg() const;

		~vertex_format() = default;
		vertex_format(const config&);

	private:

		config _cfg;
	};
}

#define IGPU_VERT_FORMAT_OF_(V, ...)\
		igpu::vertex_format::config({sizeof(V), { IGPU_VERT_FORMAT_OF_ATTRIBS(V, __VA_ARGS__) } })
#define IGPU_VERT_FORMAT_OF_ATTRIBS(V, ...) VA_DISTRIBUTE_OP_ARGS_( IGPU_VERT_FORMAT_OF_ATTRIB__ARGS_EXPAND, (V) , __VA_ARGS__)
#define IGPU_VERT_FORMAT_OF_ATTRIB__ARGS_EXPAND(V,A) { {#A, to_components((*((V*)0)).A) }, ((size_t)&(((V *)0)->A))},
