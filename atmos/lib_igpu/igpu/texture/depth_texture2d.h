
#pragma once

#include <igpu/texture/depth_format.h>
#include <igpu/texture/depth_target.h>
#include <igpu/texture/sampler.h>
#include <glm/vec2.hpp>

namespace igpu
{
    class depth_texture2d : public depth_target
    {
    public:
        
		struct config
		{
			std::string name;
			depth_format format;
			glm::ivec2 res;
			sampler sampler;
		};

		virtual const config& cfg() const = 0;
        
        virtual ~depth_texture2d() {}
        
    protected:
        
        depth_texture2d();
        
    protected:
        
        depth_texture2d (const depth_texture2d&) = delete;
        depth_texture2d& operator= (const depth_texture2d&) = delete;
    };
}
