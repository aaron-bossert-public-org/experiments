
#pragma once

#include <igpu/buffer/buffer.h>

namespace igpu
{
	class compute_buffer : public buffer
	{
	public:

		struct config
		{
			buffer_usage usage;
		};

		virtual const config& cfg() const = 0;
	};
}
