
#pragma once

#include "framework/meta_programming/va_macro.h"

#include "glm/fwd.hpp"

namespace igpu
{
	ENUM_SERIALIZABLE(

		components,
		DEFAULT( FLOAT4 ),

		( UNDEFINED, 0xFFFFFFFF ),
		( FLOAT1, 0 ),
		( FLOAT2, 1 ),
		( FLOAT3, 2 ),
		( FLOAT4, 3 ) );

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
