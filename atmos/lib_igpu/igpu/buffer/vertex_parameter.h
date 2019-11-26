
#pragma once

#include "igpu/buffer/components.h"

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
		};

		virtual const config& cfg() const = 0;

		virtual ~vertex_parameter() = default;
	};
}
