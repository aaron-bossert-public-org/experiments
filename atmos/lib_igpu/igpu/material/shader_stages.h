
#pragma once

#include <framework/meta_programming/va_macro.h>

namespace igpu
{
	ENUM_FLAGS_SERIALIZABLE_TYPE(

		shader_stages, uint32_t,
		DEFAULT(NONE),

		(NONE, 0),
		(VERTEX, 1 << 0),
		(TESSELLATION_CONTROL, 1 << 1),
		(TESSELLATION_EVALUATION, 1 << 2),
		(GEOMETRY, 1 << 3),
		(FRAGMENT, 1 << 4),
		(COMPUTE, 1 << 5),
		(ALL, (1 << 6) - 1)
	);
}