
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
        
        virtual ~depth_texture2d() = default;
        
    protected:
        
        depth_texture2d();
        
    protected:
        
        depth_texture2d (const depth_texture2d&) = delete;
        depth_texture2d& operator= (const depth_texture2d&) = delete;
    };
}
