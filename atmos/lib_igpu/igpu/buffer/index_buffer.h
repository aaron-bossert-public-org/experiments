
#pragma once

#include <igpu/buffer/buffer.h>
#include <igpu/buffer/index_format.h>

namespace igpu
{
    class index_buffer : public buffer
    {
    public:

		struct config : buffer::config
		{
			index_format format;
		};

		virtual const config& cfg() const = 0;

		size_t count() const
		{
			return byte_size() / bytes_per_index(cfg().format);
		}
    };   
}
