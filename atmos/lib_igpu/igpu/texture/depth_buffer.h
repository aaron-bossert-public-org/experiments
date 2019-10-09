
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
        
        const config& cfg() const;
        
        const glm::ivec2& resolution() const override;
        
        static bool validate(const config&);
        
        virtual ~depth_buffer() {}
        
    protected:
        
        depth_buffer(const config&);
        
    protected:
        
		config _cfg;
        
        depth_buffer (const depth_buffer&) = delete;
        depth_buffer& operator= (const depth_buffer&) = delete;
    };
}

