
#pragma once

#include <framework/meta_programming/va_macro.h>

#include <string>

namespace igpu
{
	ENUM_SERIALIZABLE(

		components, DEFAULT(FLOAT4),

		(FLOAT1, 0),
		(FLOAT2, 1),
		(FLOAT3, 2),
		(FLOAT4, 3)
	);
}
