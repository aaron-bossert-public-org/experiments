
#pragma once

#include "igpu/buffer/buffer.h"
#include "igpu/shader/vertex_parameter.h"

#include <vector>

#define IGPU_VERT_CFG_OF( U, V, ... ) IGPU_VERT_CFG_OF_( U, V, __VA_ARGS__ )

// example usage of IGPU_VERT_CFG_OF:

// struct Vertex
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
// vertex_format format = IGPU_VERT_CFG_OF(Vertex, pos, col, uv0);

namespace igpu
{
	class vertex_buffer : public buffer
	{
	public:
		struct attribute
		{
			std::string name;
			components components;
			uint32_t offset = 0;
		};

		struct config
		{
			uint32_t stride = 0;
			std::vector< attribute > attributes;
			memory_type memory = memory_type::WRITE_COMBINED;
		};

		virtual const config& cfg() const = 0;

		virtual ~vertex_buffer() = default;
	};
}

#define IGPU_VERT_CFG_OF_( V, ... ) \
	igpu::vertex_buffer::config(    \
		{ sizeof( V ), { IGPU_VERT_CFG_OF_ATTRIBS( V, __VA_ARGS__ ) } } )

#define IGPU_VERT_CFG_OF_ATTRIBS( V, ... )    \
	VA_DISTRIBUTE_OP_ARGS_(                   \
		IGPU_VERT_CFG_OF_ATTRIB__ARGS_EXPAND, \
		( V ),                                \
		__VA_ARGS__ )

#define IGPU_VERT_CFG_OF_ATTRIB__ARGS_EXPAND( V, A ) \
	{ { #A, to_components( ( *( (V*)0 ) ).A ) },     \
	  ( ( size_t ) & ( ( (V*)0 )->A ) ) },
