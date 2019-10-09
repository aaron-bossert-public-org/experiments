
#pragma once

#include <framework/meta_programming/va_macro.h>

namespace igpu
{
	ENUM_SERIALIZABLE(

		depth_format, DEFAULT(D24S8),

		(D24S8, 0),
		(D32, 1),
		(D16, 2)
	);
}


