
#pragma once

#include "framework/meta_programming/va_macro.h"

namespace igpu
{
	ENUM_SERIALIZABLE(

		index_format,
		DEFAULT( UNDEFINED ),

		( UNDEFINED, 0 ),
		( UNSIGNED_SHORT, 1 ),
		( UNSIGNED_INT, 2 ) );

	size_t bytes_per_index( index_format );
}
