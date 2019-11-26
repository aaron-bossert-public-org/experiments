#pragma once

#include "igpu/utility/utility_types.h"

#include "framework/meta_programming/va_macro.h"

#include "glm/glm.hpp"
#include <memory>
#include <string>
#include <vector>

namespace igpu
{
	class buffer;
	class context;
	class geometry;
}

namespace dcs_utils
{
	ENUM_FLAGS_SERIALIZABLE(

		model_flags,
		DEFAULT( NOTHING ),

		( NOTHING, 0 ),
		( GEN_NORMALS, 1 << 0 ),
		( GEN_TANGENTS, 1 << 1 ),
		( CLOCKWISE_WINDING, 1 << 2 ),
		( FLIP_TEXCOORD_Y, 1 << 3 ),
		( PRE_TRANSFORM, 1 << 4 ),
		( JOIN_IDENTICAL_VERTICES, 1 << 5 ) );

	struct model
	{
		struct mesh
		{
			size_t index_start = ~0U;
			size_t index_count = ~0U;
			size_t base_vertex = ~0U;
			utility::aabb aabb;
		};

		std::vector< mesh > meshes;
		std::shared_ptr< igpu::geometry > geometry;
	};
	bool load_buffer( const std::string_view& path, igpu::buffer* out_buffer );

	std::vector< uint32_t > load_mem( const std::string_view& path );

	model load_model(
		igpu::context*,
		const std::string_view& path,
		model_flags );
}
