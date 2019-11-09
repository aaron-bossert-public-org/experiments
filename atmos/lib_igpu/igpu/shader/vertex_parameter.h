
#pragma once

#include "igpu/buffer/components.h"

#include "framework/meta_programming/va_macro.h"

#include <string>

namespace igpu
{
	class vertex_parameter
	{
	public:
		struct config
		{
			std::string name;
			components components;
			size_t location = (size_t)-1;
		};

		virtual const config& cfg() const = 0;

		virtual ~vertex_parameter() = default;
	};
}
