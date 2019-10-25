
#pragma once

#include <igpu/buffer/buffer_usage.h>

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

		virtual void map(size_t byte_size, buffer_view_base*) = 0;

		virtual void unmap() = 0;

		virtual size_t byte_size() const = 0;

	protected:

		buffer() = default;
		buffer(const buffer&) = delete;
		buffer& operator= (const buffer&) = delete;
	};
}
