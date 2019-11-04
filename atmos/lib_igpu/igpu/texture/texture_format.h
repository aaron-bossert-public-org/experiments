
#pragma once

#include "framework/meta_programming/va_macro.h"

namespace igpu
{
	ENUM_SERIALIZABLE(

		texture_format,
		DEFAULT( UNDEFINED ),

		( INT_R8, 1 ),
		( INT_R8G8, 2 ),
		( INT_R8G8B8, 3 ),
		( INT_R8G8B8A8, 4 ),
		( INT_R16, 5 ),
		( INT_R16G16, 6 ),
		( INT_R16G16B16, 7 ),
		( INT_R16G16B16A16, 8 ),
		( PVR1_2BPP_RGB, 9 ),
		( PVR1_2BPP_RGBA, 10 ),
		( PVR1_4BPP_RGB, 11 ),
		( PVR1_4BPP_RGBA, 12 ),
		( PVR2_2BPP, 13 ),
		( PVR2_4BPP, 14 ),
		( ETC1_RGB, 15 ),
		( DXT_1_RGBA, 16 ),
		( DXT_3_RGBA, 17 ),
		( DXT_5_RGBA, 18 ),

		( UNDEFINED, 0 ) );
}
