
#pragma once

#include "framework/meta_programming/va_macro.h"

namespace igpu
{
	ENUM_SERIALIZABLE(

		color_format,
		DEFAULT( INT_R8G8B8A8 ),

		( INT_R8G8B8A8, 0 ),
		( INT_R5G6B5, 1 ),
		( INT_R16, 2 ),
		( FLOAT_R16, 3 ),
		( FLOAT_R16G16, 4 ),
		( FLOAT_R32, 5 ),
		( FLOAT_R32G32, 6 ) );
}
