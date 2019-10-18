
#pragma once

#include <igpu/texture/color_format.h>
#include <igpu/texture/sampler.h>
#include <glm/vec2.hpp>

namespace igpu
{
    class color_target
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

		virtual ~color_target() {}
        
    protected:
		color_target() = default;
        color_target (const color_target&) = delete;
        color_target& operator= (const color_target&) = delete;
    };
}
