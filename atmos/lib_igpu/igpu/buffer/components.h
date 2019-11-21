
#pragma once

#include "framework/meta_programming/va_macro.h"

#include "glm/gtc/vec1.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace igpu
{
	ENUM_SERIALIZABLE(

		components,
		DEFAULT( UNDEFINED ),

		( UNDEFINED, 0 ),
		( FLOAT1, 1 ),
		( FLOAT2, 2 ),
		( FLOAT3, 3 ),
		( FLOAT4, 4 ) );

	constexpr components to_components( const glm::vec1& )
	{
		return components::FLOAT1;
	}
	constexpr components to_components( const glm::vec2& )
	{
		return components::FLOAT2;
	}
	constexpr components to_components( const glm::vec3& )
	{
		return components::FLOAT3;
	}
	constexpr components to_components( const glm::vec4& )
	{
		return components::FLOAT4;
	}
}
