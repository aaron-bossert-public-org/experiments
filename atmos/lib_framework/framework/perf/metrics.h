
#pragma once

#include "framework/meta_programming/va_macro.h"

#include <string_view>

namespace perf
{
	ENUM_SERIALIZABLE(

		category,
		DEFAULT( UNDEFINED ),

		( UNDEFINED, 0 ),
		( CLEAR_DRAW_TARGET, 1 ),
		( DRAW_CALL_COUNT, 2 ),
		( POLY_COUNT, 3 ),
		( SWITCH_DRAW_TARGET, 4 ),
		( SWITCH_SHADERS, 5 ),
		( SWITCH_RENDER_STATES, 6 ),
		( SWITCH_TEXTURES, 7 ),
		( SWITCH_PARAMETERS, 8 ),
		( SWITCH_GEOMETRY, 9 ),
		( DISPATCH_TIME, 10 ),
		( CPU_MEM_USAGE, 11 ),
		( GPU_MEM_USAGE, 12 ),
		( CPU_GPU_SHARED_MEM_USAGE, 13 ) );

	class metric
	{
	public:
		metric( category, const std::string_view& );
		void add( double )
		{}
		void add( float )
		{}
		void add( size_t )
		{}
		void add( int )
		{}
		void reset()
		{}
	};

	class time_metric
	{
	public:
		class scope
		{};

		time_metric( category, const std::string_view& sub_category );

		scope time_scope()
		{
			return scope();
		}
	};
}