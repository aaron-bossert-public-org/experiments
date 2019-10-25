
#pragma once

#include <igpu/buffer/buffer.h>
#include <igpu/buffer/vertex_format.h>

namespace igpu
{
	class vertex_buffer : public buffer
	{
	public:

		struct config : buffer::config
		{
			vertex_format::config format;
		};

		virtual const config& cfg() const = 0;
	};
}
