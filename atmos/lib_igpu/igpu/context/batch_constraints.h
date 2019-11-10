
#pragma once

#include "igpu/context/parameter_constraints.h"

namespace igpu
{
	class batch_constraints : public parameter_constraints
	{
	public:
		using parameter_constraints::parameter_constraints;

		virtual ~batch_constraints() = default;
	};
}
