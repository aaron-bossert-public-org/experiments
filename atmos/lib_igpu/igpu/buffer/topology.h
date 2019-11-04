
#pragma once

#include "framework/meta_programming/va_macro.h"

namespace igpu
{
	ENUM_SERIALIZABLE(

		topology,
		DEFAULT( TRIANGLE_LIST ),

		( POINT_LIST, 0 ),
		( LINE_LIST, 1 ),
		( LINE_STRIP, 2 ),
		( TRIANGLE_LIST, 3 ),
		( TRIANGLE_STRIP, 4 ),

		///	TBD: not all APIs support triangle fan
		( TRIANGLE_FAN, 5 ),
		( UNDEFINED, (uint32_t)-1 ) );

	size_t polycount( topology topology, const int32_t index_count );
}
