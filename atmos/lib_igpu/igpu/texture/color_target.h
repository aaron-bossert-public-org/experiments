
#pragma once

#include <glm/fwd.hpp>

namespace igpu
{
    class color_target
    {
    public:
        
        virtual void attach() = 0;
        
		virtual ~color_target() = 0;
        
        virtual const glm::ivec2& resolution() const = 0;
        
    protected:
		color_target() = default;
        color_target (const color_target&) = delete;
        color_target& operator= (const color_target&) = delete;
    };
}
