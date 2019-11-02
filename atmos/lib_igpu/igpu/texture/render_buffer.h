
#pragma once

#include <igpu/texture/color_format.h>
#include <igpu/texture/sampler.h>
#include <glm/vec2.hpp>

namespace igpu
{
    class render_buffer
    {
	public:

		struct config
		{
			std::string name;
			color_format format;
			sampler sampler;
			glm::ivec2 res;
		};

		virtual const config& cfg() const = 0;

		virtual ~render_buffer() = default;
        
    protected:
		render_buffer() = default;
		render_buffer(const render_buffer&) = delete;
		render_buffer& operator= (const render_buffer&) = delete;
    };
}
