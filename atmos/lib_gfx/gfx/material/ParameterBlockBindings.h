
#pragma once

#include "igpu/material/program.h"
#include "gfx/material/parameter_block.h"

namespace gfx
{
    class parameter_block_bindings final
    {
    public:
        
        parameter_block_bindings(const parameter_block&);
        
        const parameter_block& parameter_block() const;
        
        const std::vector<parameter_binding>& parameter_bindings(const igpu::program&);
        
    private:
        
        igpu::program_binding_id _program_binding_id = (size_t)-1;
        
        parameter_block_binding_id _parameter_block_binding_id = -1;
        
        const parameter_block& _parameter_block;
        
        std::vector<parameter_binding> _parameter_bindings;
    };
}
