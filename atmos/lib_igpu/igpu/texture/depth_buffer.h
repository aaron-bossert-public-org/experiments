
#pragma once

#include <igpu/texture/depth_format.h>
#include <igpu/texture/sampler.h>
#include <igpu/texture/depth_target.h>

#include <glm/vec2.hpp>

namespace igpu
{
    class depth_buffer : public depth_target
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
        
		virtual ~depth_buffer() = 0;
        
    protected:
        
		depth_buffer() = default;
        depth_buffer (const depth_buffer&) = delete;
        depth_buffer& operator= (const depth_buffer&) = delete;
    };
}

