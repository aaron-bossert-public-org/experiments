
#pragma once

#include <igpu/buffer/buffer_usage.h>

class buffer_view_base;

namespace igpu
{
	class buffer
	{
	public:

		virtual ~buffer() = 0;

		virtual void map(buffer_view_base&, size_t byte_size) = 0;

		virtual void unmap() = 0;

		virtual size_t byte_size() const = 0;

	protected:

		buffer() = default;
		buffer(const buffer&) = delete;
		buffer& operator= (const buffer&) = delete;
	};
}
