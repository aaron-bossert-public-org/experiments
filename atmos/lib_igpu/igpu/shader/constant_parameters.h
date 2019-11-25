
#pragma once

#include "igpu/shader/constant_parameter.h"

#include <vector>

namespace igpu
{
	class constant_parameters
	{
	public:
		static constexpr size_t MAX_COUNT = 64;

		struct config
		{
			std::vector< constant_parameter::config > constants;
		};

		virtual const config& cfg() const = 0;
	};
}