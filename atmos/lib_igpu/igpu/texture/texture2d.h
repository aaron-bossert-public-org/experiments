
#pragma once

#include <igpu/texture/texel_buffer2d.h>

namespace igpu
{
    class texture2d
    {
    public:

		using config = texel_buffer2d::config;
        
        virtual ~texture2d() {}
        
        virtual const config& cfg() const = 0;
        
    protected:
        
		texture2d() = default;
        texture2d (const texture2d&) = delete;
        texture2d& operator= (const texture2d&) = delete;
    };
}
