
#pragma once

#include "igpu/buffer/buffer.h"

namespace igpu
{
	class compute_buffer : public buffer
	{
	public:
		struct config
		{
			buffer_mapping mapping = buffer_mapping::WRITE_COMBINE;
		};
	};
}
