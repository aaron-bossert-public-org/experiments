
#pragma once

#include <igpu/texture/depth_format.h>
#include <igpu/texture/sampler.h>
#include <glm/vec2.hpp>
#include <string>

namespace igpu
{
    class depth_target
    {
    public:
		
		struct config
		{
			std::string name;
			depth_format format;
			sampler sampler;
			glm::ivec2 res;
		};

		virtual const config& cfg() const = 0;

		virtual ~depth_target() {}
        
    protected:
        depth_target() {}
        
        depth_target (const depth_target&) = delete;
        depth_target& operator= (const depth_target&) = delete;
    };
}
