
#pragma once

#include <glm/vec2.hpp>
#include <memory>

namespace igpu
{
    class depth_target;
    class color_target;
    
    class draw_target
    {
    public:
        const std::shared_ptr<color_target>& color() const;
        
        const std::shared_ptr<depth_target>& depth() const;
        
        glm::ivec2 res() const;
        
        virtual ~draw_target() = default;
        
    protected:
        
        draw_target(const std::shared_ptr<color_target>&,
                   const std::shared_ptr<depth_target>&);
        
        std::shared_ptr<color_target> _color;
        std::shared_ptr<depth_target> _depth;
        
        draw_target (const draw_target&) = delete;
        draw_target& operator= (const draw_target&) = delete;
    };
}
