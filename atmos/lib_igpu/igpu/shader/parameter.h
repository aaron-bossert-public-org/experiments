
#pragma once

#include "framework/meta_programming/va_macro.h"

#include <string>

namespace igpu
{
	ENUM_FLAGS_SERIALIZABLE(

		decorator,
		DEFAULT( NOTHING ),

		( NOTHING, 0 ),
		( READABLE, 1 << 0 ),
		( WRITABLE, 1 << 1 ),
		( ALL, ( 1 << 2 ) - 1 ) );

	class parameter
	{
	public:
		ENUM_SERIALIZABLE(

			type,
			DEFAULT( UNDEFINED ),

			( UNDEFINED, 0 ),
			( STORAGE_BUFFER, 1 ),
			( UNIFORM_BUFFER, 2 ),
			( SAMPLER2D, 3 ) );

		struct config
		{
			std::string name;
			type type = type::UNDEFINED;
			decorator decorators = decorator::NOTHING;
			size_t array_size = 1;
		};

		virtual const config& cfg() const = 0;

		virtual ~parameter() = default;
	};
}