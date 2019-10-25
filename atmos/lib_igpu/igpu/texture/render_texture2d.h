
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

		virtual const config& cfg() const;

		virtual ~render_texture2d() = default;
        
    protected:
        
		render_texture2d() = default;
        render_texture2d (const render_texture2d&) = delete;
        render_texture2d& operator= (const render_texture2d&) = delete;
    };
}

