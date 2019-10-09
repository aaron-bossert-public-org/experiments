
#pragma once

#include <igpu/resource/resource_usage.h>
#include <igpu/resource/vertex_format.h>

#include <memory>

class buffer_view_base;

namespace igpu
{
	class vertex_resource
	{
	public:

		struct config
		{
			vertex_format::config format;
			resource_usage usage;
		};

		virtual const config& cfg() const = 0;

		virtual ~vertex_resource() = 0;

		virtual void map(buffer_view_base&, size_t count) = 0;

		virtual void map_subrange(buffer_view_base&, size_t offset, size_t count) = 0;

		virtual void unmap(buffer_view_base&) = 0;

		virtual size_t capacity() const = 0;

	protected:

		vertex_resource() = default;
		vertex_resource(const vertex_resource&) = delete;
		vertex_resource& operator= (const vertex_resource&) = delete;
	};
}
