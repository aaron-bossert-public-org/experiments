
#pragma once

#include <glm/fwd.hpp>

namespace igpu
{
    class depth_target
    {
    public:
        virtual void attach() = 0;

		virtual ~depth_target();
        
        virtual const glm::ivec2& resolution() const = 0;
        
    protected:
        depth_target() {}
        
        depth_target (const depth_target&) = delete;
        depth_target& operator= (const depth_target&) = delete;
    };
}
