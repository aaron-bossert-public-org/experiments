
#pragma once

#include <cstdint>
#include <memory>

namespace igpu
{
	struct compute_parameters
	{
		uint32_t group_count_x = 1;
		uint32_t group_count_y = 1;
		uint32_t group_count_z = 1;
	};
}
