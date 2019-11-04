
#pragma once

#include "igpu/material/primitive.h"

namespace igpu
{
    class parameter;
    class program;
}

namespace gfx
{
    class parameter_binding
    {
    public:
        void set(const std::string&, const igpu::primitive*, const igpu::program&);
        
        const std::string& name() const;
        
        const igpu::primitive* primitive() const;
        
        const igpu::parameter* parameter() const;
        
    private:
        
        std::string _name;
        const igpu::primitive* _primitive = nullptr;
        const igpu::parameter* _parameter = nullptr;
    };
}
