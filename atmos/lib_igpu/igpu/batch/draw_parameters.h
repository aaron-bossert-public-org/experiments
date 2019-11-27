
#pragma once

#include <cstdint>
#include <memory>

namespace igpu
{
	class compute_buffer;

	struct draw_parameters
	{
		uint32_t index_count = 0;
		uint32_t instance_count = 0;
		uint32_t first_index = 0;
		int32_t base_vertex = 0;
		uint32_t first_instance = 0;
	};

	struct draw_indirect_parameters
	{
		uint32_t offset = 0;
		uint32_t count = 0;
		uint32_t stride = sizeof draw_parameters;
		std::shared_ptr< compute_buffer > buffer;
	};
}
