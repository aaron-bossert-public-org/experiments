
#pragma once

#include <glm/fwd.hpp>

namespace igpu
{
    class color_target
    {
    public:
        
        virtual void attach() = 0;
        
		virtual ~color_target();
        
        virtual const glm::ivec2& resolution() const = 0;
        
    protected:
        color_target() {}
        
        color_target (const color_target&) = delete;
        color_target& operator= (const color_target&) = delete;
    };
}
