
#pragma once

#include <igpu/buffer/buffer_usage.h>
#include <framework/perf/metrics.h>

namespace igpu
{
    class gl_buffer
    {
    public:

		gl_buffer(
			buffer_usage usage,
			unsigned gl_target);

        ~gl_buffer();
        
		void map(
			size_t byte_size,
			buffer_view_base*);

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

		const config& cfg() const override
		{
			return _cfg;
		}

		void map(size_t byte_size, buffer_view_base* out_buffer_view) override
		{
			_gl_buffer.map(byte_size, out_buffer_view);
		}

		void unmap() override
		{
			_gl_buffer.unmap();
		}

		size_t byte_size() const override
		{
			return _gl_buffer.byte_size();
		}

		unsigned gl_handle() const override;

		template <typename... ARGS>
		static std::unique_ptr < gl_buffer_t > make(
			const config& cfg,
			unsigned gl_target,
			const ARGS& ... args)
		{
			if (!is_valid(cfg.usage))
			{
				LOG_CRITICAL("invalid usage:%d", (int)cfg.usage);
			}
			else
			{
				return std::unique_ptr<gl_buffer_t>(
					new gl_buffer_t(
						cfg,
						gl_target,
						args...));
			}

			return nullptr;
		}

	protected:

		template <typename... ARGS>
		gl_buffer_t(
			const config& cfg,
			unsigned gl_target,
			const ARGS& ... args)
			: T(args...)
			, _cfg(cfg)
			, _gl_buffer(
				cfg.usage,
				gl_target)
		{
		}

	private:

		const config _cfg;
		gl_buffer _gl_buffer;
	};
}
