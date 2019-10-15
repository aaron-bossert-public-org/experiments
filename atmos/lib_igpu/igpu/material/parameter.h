
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
        
        const config& cfg() const;

		size_t constraint_index() const;
        
    protected:

		parameter(const config&, size_t constraint_index);
		parameter(const parameter&) = default;
		parameter& operator= (const parameter&) = default;

    protected:
        
        config _cfg;
		size_t _constraint_index;
    };
}