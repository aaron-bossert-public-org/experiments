
#pragma once

#include <igpu/resource/resource_usage.h>

#include <memory>

class buffer_view_base;

namespace igpu
{
	class compute_resource
	{
	public:

		struct config
		{
			resource_usage usage;
		};

		virtual const config& cfg() const = 0;

		virtual ~compute_resource() = 0;

		virtual void map(buffer_view_base&, size_t count) = 0;

		virtual void map_subrange(buffer_view_base&, size_t offset, size_t count) = 0;

		virtual void unmap(buffer_view_base&) = 0;

		virtual size_t capacity() const = 0;

	protected:

		compute_resource() = default;
		compute_resource(const compute_resource&) = delete;
		compute_resource& operator= (const compute_resource&) = delete;
	};
}
