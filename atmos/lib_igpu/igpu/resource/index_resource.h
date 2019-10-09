
#pragma once

#include <igpu/resource/resource_usage.h>
#include <igpu/resource/index_format.h>

#include <memory>

class buffer_view_base;

namespace igpu
{
    class index_resource
    {
    public:

		struct config
		{
			index_format format;
			resource_usage usage;
		};

		virtual const config& cfg() const = 0;

		virtual ~index_resource() = 0;
		
		virtual void map(buffer_view_base&, size_t count) = 0;

		virtual void map_subrange(buffer_view_base&, size_t offset, size_t count) = 0;

		virtual void unmap(buffer_view_base&) = 0;

		virtual size_t capacity() const = 0;
        
    protected:

		index_resource() = default;
		index_resource(const index_resource&) = delete;
		index_resource& operator= (const index_resource&) = delete;
    };   
}
