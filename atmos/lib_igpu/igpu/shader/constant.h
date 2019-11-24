
#pragma once

#include "glm/gtc/vec1.hpp"
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
	class constant
	{
	public:
		using variant_t = std::variant<
			glm::ivec1,
			glm::ivec2,
			glm::ivec3,
			glm::ivec4,
			glm::vec1,
			glm::vec2,
			glm::vec3,
			glm::vec4 >;

		std::string name;

		variant_t value;

		size_t hash() const;

		ptrdiff_t compare( const constant& ) const;
	};
#pragma warning( pop )
}
