
#pragma once

#include <igpu/buffer/buffer_usage.h>
#include <framework/perf/metrics.h>

namespace igpu
{
    class gl_buffer
    {
    public:

		gl_buffer(
			const std::string_view& metric,
			buffer_usage usage,
			unsigned gl_target);

        ~gl_buffer();
        
		void map(
			buffer_view_base&,
			size_t byte_size);

		void unmap();
		
		void release();

		unsigned gl_handle() const;

		size_t byte_size() const;

		buffer_usage usage() const;
        
    private:

		const unsigned _gl_target;
		const unsigned _gl_handle;
		const unsigned _gl_usage;
		const unsigned _gl_access;
		const buffer_usage _usage;

		size_t _byte_size;
		void* _mapped;
        perf::metric _gpu_mem_metric;
    };

	template < typename T >
	class gl_buffer_t : public T
	{
	public:

		using config = typename T::config;

		~gl_buffer_t() override {};

		const config& cfg() const override
		{
			return _cfg;
		}

		void map(buffer_view_base& buffer_view, size_t byte_size) override
		{
			_gl_buffer.map(buffer_view, byte_size);
		}

		void unmap() override
		{
			_gl_buffer.unmap();
		}

		size_t byte_size() const override
		{
			return _gl_buffer.byte_size();
		}

		unsigned gl_handle() const
		{
			return _gl_buffer.gl_handle();
		}

	protected:

		gl_buffer_t(
			const config& cfg,
			const std::string_view& metric,
			buffer_usage usage,
			unsigned gl_target)
			: _cfg(cfg)
			, _gl_buffer(
				metric,
				usage,
				gl_target)
		{
		}

	private:

		const config _cfg;
		gl_buffer _gl_buffer;
	};
}
