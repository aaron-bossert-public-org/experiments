
#pragma once

#include "framework/meta_programming/va_macro.h"

namespace igpu
{
	ENUM_SERIALIZABLE(

		topology,
		DEFAULT( UNDEFINED ),

		( UNDEFINED, 0 ),
		( POINT_LIST, 1 ),
		( LINE_LIST, 2 ),
		( LINE_STRIP, 3 ),
		( TRIANGLE_LIST, 4 ),
		( TRIANGLE_STRIP, 5 ),
		( TRIANGLE_FAN, 6 ) );

	size_t polycount( topology topology, const int32_t index_count );
}
