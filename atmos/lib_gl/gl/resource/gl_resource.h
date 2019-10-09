
#pragma once

#include <igpu/resource/resource_usage.h>
#include <framework/perf/metrics.h>

namespace igpu
{
    class gl_resource
    {
    public:

		gl_resource(
			const std::string_view& metric,
			resource_usage usage,
			unsigned gl_target);

        ~gl_resource();
        
		void map(
			buffer_view_base&,
			size_t count);

		void map_subrange(
			buffer_view_base&,
			size_t offset,
			size_t count);

		void unmap(
			buffer_view_base& error_check_mapped);

		void unmap();
		
		void release();

		unsigned gl_handle() const;

		size_t capacity() const;

		resource_usage usage() const;
        
    private:

		const unsigned _gl_target;
		const unsigned _gl_handle;
		const unsigned _gl_usage;
		const unsigned _gl_access;
		const resource_usage _usage;

		size_t _capacity;
		void* _mapped;
        perf::metric _gpu_mem_metric;
    };
}
