
#pragma once

#include <framework/meta_programming/va_macro.h>
#include <framework/utility/buffer_view.h>

#include <memory>
#include <stdint.h>

namespace igpu
{
	ENUM_SERIALIZABLE(

		resource_usage, DEFAULT(STATIC),

		(STATIC, 0),
		(DYNAMIC, 1)
	);
}
