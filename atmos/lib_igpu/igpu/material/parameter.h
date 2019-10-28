
#pragma once

#include <framework/meta_programming/va_macro.h>
#include <string>

namespace igpu
{   
    class parameter
    {
    public:

		ENUM_SERIALIZABLE(

			type, DEFAULT(COMPUTE_BUFFER),

			(COMPUTE_BUFFER, 0),
			(TEXTURE2D, 1)
		);

		struct config
		{
			std::string name;
			type type;
		};
        
        virtual const config& cfg() const = 0;

		virtual ~parameter() = default;
        
	protected:
		
		parameter() = default;

		parameter(const parameter&) = delete;
		parameter& operator= (const parameter&) = delete;
    };
}