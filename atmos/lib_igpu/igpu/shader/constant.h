
#pragma once

#include "framework/meta_programming/va_macro.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include <string>
#include <variant>

namespace igpu
{
#pragma warning( push )
#pragma warning( disable : 4324 ) // structure was padded due to alignment
								  // specifier	class constant

	ENUM_SERIALIZABLE(
		constant_type,
		DEFAULT( UNDEFINED ),

		( UNDEFINED, 0 ),
		( INT32, 1 ),
		( IVEC2, 2 ),
		( IVEC3, 3 ),
		( IVEC4, 4 ),
		( F32, 5 ),
		( VEC2, 6 ),
		( VEC3, 7 ),
		( VEC4, 8 ) );

	class constant
	{
	public:
		using variant_t = std::variant<
			int32_t,
			glm::ivec2,
			glm::ivec3,
			glm::ivec4,
			float,
			glm::vec2,
			glm::vec3,
			glm::vec4 >;

		std::string name;

		variant_t value;

		constant_type type() const;

		size_t hash() const;

		ptrdiff_t compare( const constant& ) const;
	};

	constant_type to_constant_type( const int32_t& );
	constant_type to_constant_type( const glm::ivec2& );
	constant_type to_constant_type( const glm::ivec3& );
	constant_type to_constant_type( const glm::ivec4& );
	constant_type to_constant_type( const float& );
	constant_type to_constant_type( const glm::vec2& );
	constant_type to_constant_type( const glm::vec3& );
	constant_type to_constant_type( const glm::vec4& );

#pragma warning( pop )
}
