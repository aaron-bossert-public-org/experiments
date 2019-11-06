
#pragma once

#include "framework/meta_programming/va_macro.h"

class buffer_view_base;

namespace igpu
{
	ENUM_SERIALIZABLE(

		buffer_mapping,
		DEFAULT( WRITE_COMBINE ),

		( WRITE_COMBINE, 1 ),	 // contents are undefined each map(...)
		( PRESERVE, 2 ) );		 // contents are preserved each map(...)

	class buffer
	{
	public:
		virtual ~buffer() = default;

		virtual void map( buffer_view_base* ) = 0;

		virtual void unmap() = 0;
	};
}
