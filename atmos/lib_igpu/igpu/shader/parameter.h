
#pragma once

#include "framework/meta_programming/va_macro.h"
#include <string>

namespace igpu
{   
    class parameter
    {
    public:

		ENUM_SERIALIZABLE(

			type, DEFAULT(UNDEFINED),

			(UNDEFINED, 0xFFFFFFFF),
			(COMPUTE_BUFFER, 0),
			(TEXTURE2D, 1)
		);

		struct config
		{
			std::string name;
			type type;
			size_t array_size;
		};
        
        virtual const config& cfg() const = 0;

		virtual ~parameter() = default;
    };
}