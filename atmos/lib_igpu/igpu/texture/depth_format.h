
#pragma once

#include "framework/meta_programming/va_macro.h"

namespace igpu
{
	ENUM_SERIALIZABLE(

		depth_format,
		DEFAULT( UNDEFINED ),

		( UNDEFINED, 0 ),
		( D24S8, 1 ),
		( D32, 2 ),
		( D16, 3 ) );
}
