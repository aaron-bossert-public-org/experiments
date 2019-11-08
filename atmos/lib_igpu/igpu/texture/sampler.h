
#pragma once

#include "framework/meta_programming/va_macro.h"

#include <string>

namespace igpu
{
	struct sampler
	{
	public:
		ENUM_SERIALIZABLE(

			filter,
			DEFAULT( UNDEFINED ),

			( UNDEFINED, 0 ),
			( NEAREST, 1 ),
			( LINEAR, 2 ) );

		ENUM_SERIALIZABLE(

			address,
			DEFAULT( UNDEFINED ),

			( UNDEFINED, 0 ),
			( CLAMP, 1 ),
			( WRAP, 2 ),
			( MIRROR, 3 ) );

		filter min_filter;
		filter mag_filter;
		address addressu;
		address addressv;
	};
}
