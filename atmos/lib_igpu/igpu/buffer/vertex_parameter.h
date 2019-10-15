
#pragma once

#include <framework/meta_programming/va_macro.h>
#include <igpu/buffer/components.h>

#include <string>

namespace igpu
{
	struct vertex_parameter
	{
		std::string name;
		components components;
	};
}
