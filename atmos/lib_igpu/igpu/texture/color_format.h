
#pragma once

#include "framework/meta_programming/va_macro.h"

namespace igpu
{
	ENUM_SERIALIZABLE(

		color_format,
		DEFAULT( UNDEFINED ),

		( UNDEFINED, 0 ),
		( INT_R8G8B8A8, 1 ),
		( INT_R5G6B5, 2 ),
		( INT_R16, 3 ),
		( FLOAT_R16, 4 ),
		( FLOAT_R16G16, 5 ),
		( FLOAT_R32, 6 ),
		( FLOAT_R32G32, 7 ) );
}
