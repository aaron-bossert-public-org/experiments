
#pragma once

#include <framework/meta_programming/va_macro.h>
#include <string>

namespace igpu
{
	struct sampler
    {
	public:
		ENUM_SERIALIZABLE(

			filter, DEFAULT(NEAREST),
			(NEAREST, 0),
			(LINEAR, 1)
		);

		ENUM_SERIALIZABLE(

			address, DEFAULT(CLAMP),
			(CLAMP, 0),
			(WRAP, 1),
			(MIRROR, 2)
		);

		filter min_filter;
		filter mag_filter;
		address addressu;
		address addressv;
    };    
}
