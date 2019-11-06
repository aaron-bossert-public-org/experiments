
#pragma once

#include "igpu/buffer/buffer.h"
#include "igpu/buffer/index_format.h"

namespace igpu
{
	class index_buffer : public buffer
	{
	public:
		struct config
		{
			index_format format;
			buffer_mapping mapping = buffer_mapping::WRITE_COMBINE;
		};

		virtual const config& cfg() const = 0;

		virtual size_t element_count() const = 0;
	};
}
