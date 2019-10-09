
#pragma once

#include <igpu/texture/color_target.h>
#include <igpu/texture/color_format.h>
#include <igpu/texture/sampler.h>
#include <glm/vec2.hpp>

namespace igpu
{
    class render_texture2d : public color_target
    {
    public:

		struct config
		{
			std::string name;
			sampler sampler;
			glm::ivec2 res;
			color_format format;
		};

        const config& cfg() const;
        
        const glm::ivec2& resolution() const override;
        
        virtual ~render_texture2d() {}
        
    protected:
        
        render_texture2d(const config&);
        
    protected:
        
		config _cfg;
        
        render_texture2d (const render_texture2d&) = delete;
        render_texture2d& operator= (const render_texture2d&) = delete;
    };
}

