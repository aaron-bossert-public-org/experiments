
#pragma once

#include <igpu/texture/color_format.h>
#include <igpu/texture/depth_format.h>
#include <igpu/texture/sampler.h>
#include <glm/vec2.hpp>

namespace igpu
{
    class color_target;
    class depth_target;
    
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
        
		virtual const color_target& color() const = 0;

		virtual const depth_target& depth() const = 0;

    protected:
                
        back_buffer() = default;
		back_buffer (const back_buffer&) = delete;
        back_buffer& operator= (const back_buffer&) = delete;
    };
}
