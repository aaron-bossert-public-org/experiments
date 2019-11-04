
#pragma once

#include "framework/meta_programming/va_macro.h"

namespace igpu
{
	ENUM_SERIALIZABLE(

		index_format, DEFAULT(UNDEFINED),

		(UNSIGNED_SHORT, 0),
		(UNSIGNED_INT, 1),
		(UNDEFINED, (uint32_t)-1)
	);

	size_t bytes_per_index(index_format);
}
