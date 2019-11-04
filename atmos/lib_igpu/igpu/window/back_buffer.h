
#pragma once

#include "igpu/texture/color_format.h"
#include "igpu/texture/depth_format.h"
#include "igpu/texture/sampler.h"
#include "glm/vec2.hpp"

namespace igpu
{
    class render_buffer;
    class depth_buffer;
    
    class back_buffer
    {
    public:
        
		struct config
		{
			color_format color_format;
			depth_format depth_format;
			sampler sampler;
			glm::ivec2 res = {};
		};

        virtual const config& cfg() const = 0;
        
		virtual const render_buffer& color() const = 0;

		virtual const depth_buffer& depth() const = 0;

    protected:
                
        back_buffer() = default;
		back_buffer (const back_buffer&) = delete;
        back_buffer& operator= (const back_buffer&) = delete;
    };
}
