
#pragma once

#include "igpu/buffer/buffer.h"

namespace igpu
{
	class compute_buffer : public buffer
	{
	public:
		struct config
		{
			memory_type memory = memory_type::WRITE_COMBINED;
		};
	};
}
