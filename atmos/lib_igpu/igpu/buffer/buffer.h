
#pragma once

#include "framework/meta_programming/va_macro.h"
#include "framework/utility/buffer_view.h"

class buffer_view_base;

namespace igpu
{
	ENUM_SERIALIZABLE(

		memory_type,
		DEFAULT( UNDEFINED ),

		( UNDEFINED, 0 ),	   // contents are undefined each map(...)
		( WRITE_COMBINED, 1 ), // contents are undefined each map(...)
		( PRESERVED, 2 ) );	   // contents are preserved each map(...)

	class buffer
	{
	public:
		virtual ~buffer() = default;

		virtual void map( buffer_view_base* ) = 0;

		virtual void unmap() = 0;

		template < typename T >
		void map( T** );
	};

	template < typename T >
	void buffer::map( T** ppt )
	{
		buffer_view< T > view( 1, nullptr );
		map( &view );

		*ppt = view.data();
	}
}
