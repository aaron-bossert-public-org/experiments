
#pragma once

#include <igpu/texture/color_format.h>
#include <igpu/texture/color_target.h>
#include <igpu/texture/sampler.h>

#include <glm/vec2.hpp>

namespace igpu
{
    class color_buffer : public color_target
    {
    public:
	       
		struct config
		{
			std::string name;
			color_format format;
			sampler sampler;
			glm::ivec2 res;
		};
        
        const std::string& name() const;

		virtual const config& cfg() const = 0;
        
        virtual ~color_buffer() {}
        
    protected:
        
		color_buffer() = default;
        color_buffer (const color_buffer&) = delete;
        color_buffer& operator= (const color_buffer&) = delete;
    };
}

