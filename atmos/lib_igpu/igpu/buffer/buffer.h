
#pragma once

#include "igpu/buffer/buffer_usage.h"

class buffer_view_base;

namespace igpu
{
	class buffer
	{
	public:

		struct config
		{
			buffer_usage usage;
		};

		virtual const config& cfg() const = 0;

		virtual ~buffer() = default;

		virtual void map(buffer_view_base*) = 0;

		virtual void unmap() = 0;
	};
}
