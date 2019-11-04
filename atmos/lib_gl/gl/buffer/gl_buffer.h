
#pragma once

#include "igpu/buffer/buffer.h"

#include "framework/perf/metrics.h"

namespace igpu
{
	class gl_buffer : public buffer
	{
	public:
		gl_buffer( const config&, unsigned gl_target );

		~gl_buffer();

		const config& cfg() const override;

		void map( buffer_view_base* ) override;

		void unmap() override;

		unsigned gl_handle() const;

		void release();

		size_t byte_size() const;

	private:
		const config _cfg;
		const unsigned _gl_target;
		const unsigned _gl_handle;
		const unsigned _gl_usage;
		const unsigned _gl_access;

		size_t _byte_size = 0;
		size_t _byte_capacity;
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

		void map( buffer_view_base* out_buffer_view ) override
		{
			_gl_buffer.map( out_buffer_view );
		}

		void unmap() override
		{
			_gl_buffer.unmap();
		}

		unsigned gl_handle() const override;

		size_t byte_size() const;

		template < typename... ARGS >
		static std::unique_ptr< gl_buffer_t > make(
			const config& cfg,
			unsigned gl_target,
			const ARGS&... args )
		{
			if ( !is_valid( cfg.usage ) )
			{
				LOG_CRITICAL( "invalid usage:%d", (int)cfg.usage );
			}
			else
			{
				return std::unique_ptr< gl_buffer_t >(
					new gl_buffer_t( cfg, gl_target, args... ) );
			}

			return nullptr;
		}

	protected:
		template < typename... ARGS >
		gl_buffer_t(
			const config& cfg,
			unsigned gl_target,
			const ARGS&... args )
			: T( args... )
			, _cfg( cfg )
			, _gl_buffer( _cfg, gl_target )
		{}

	private:
		const config _cfg;
		gl_buffer _gl_buffer;
	};
}
