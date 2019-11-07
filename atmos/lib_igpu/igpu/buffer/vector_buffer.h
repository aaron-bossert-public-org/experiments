
#pragma once

#include "igpu/buffer/buffer.h"

#include "framework/perf/metrics.h"
#include "framework/utility/buffer_view.h"

#include <memory>

namespace igpu
{
	template < typename T >
	class vector_buffer : buffer
	{
	public:
		vector_buffer() = default;

		vector_buffer( vector_buffer< T >&& other ) noexcept;

		vector_buffer( std::vector< T >&& other ) noexcept;

		vector_buffer& operator=( vector_buffer< T >&& other ) noexcept;

		vector_buffer& operator=( std::vector< T >&& other ) noexcept;

		void map( buffer_view_base* out_mapped_view ) override;

		void unmap() override;

		const buffer_view< T >& view_unmapped() const;

		std::vector< T > release();

	private:

		buffer_view< T > _view_unmapped = {};
		std::vector< T > _memory;

		perf::metric _cpu_mem_metric =
			perf::metric( perf::category::CPU_MEM_USAGE, "vector buffer" );
	};


	template < typename T >
	vector_buffer< T >::vector_buffer( vector_buffer< T >&& other ) noexcept
	{
		_memory = std::move( other._memory );
		_view_unmapped = other.view_unmapped;
		other.view_unmapped = {};
	}

	template < typename T >
	vector_buffer< T >::vector_buffer( std::vector< T >&& other ) noexcept
	{
		_memory = std::move( other );
		_view_unmapped = other.view_unmapped;
	}

	template < typename T >
	vector_buffer< T >& vector_buffer< T >::operator=(
		vector_buffer< T >&& other ) noexcept
	{
		_memory = std::move( other._memory );
		_view_unmapped = other.view_unmapped;
		other.view_unmapped = {};
		return *this;
	}

	template < typename T >
	vector_buffer< T >& vector_buffer< T >::operator=(
		std::vector< T >&& other ) noexcept
	{
		_memory = std::move( other );
		_view_unmapped = other.view_unmapped;
		return *this;
	}

	template < typename T >
	void vector_buffer< T >::map( buffer_view_base* out_mapped_view )
	{
		if ( _memory.size() && 0 == _view_unmapped.size() )
		{
			LOG_CRITICAL( "map/unmap mismatch" );
		}
		else
		{
			size_t count = ( out_mapped_view->byte_size() + sizeof( T ) - 1 ) /
				sizeof( T );

			_memory.resize( count );
			_view_unmapped = buffer_view< T >();
			_cpu_mem_metric.reset();
			_cpu_mem_metric.add( _memory.capacity() * sizeof T );

			*out_mapped_view = buffer_view_base(
				out_mapped_view->size(),
				_memory.data(),
				out_mapped_view->stride() );
		}
	}

	template < typename T >
	void vector_buffer< T >::unmap()
	{
		_view_unmapped = _memory;
	}

	template < typename T >
	const buffer_view< T >& vector_buffer< T >::view_unmapped() const
	{
		return _view_unmapped;
	}

	template < typename T >
	std::vector< T > vector_buffer< T >::release()
	{
		_cpu_mem_metric.reset();
		_view_unmapped = buffer_view< char >();
		return std::move( _memory );
	}

}