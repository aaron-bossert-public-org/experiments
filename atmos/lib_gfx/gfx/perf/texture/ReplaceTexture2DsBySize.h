
#pragma once

#include <gfx/context/draw_pass_bundle.h>

namespace igpu
{
    class texture_2d;
}

namespace gfx
{
    class program_instance;
    
    class replace_texture_2ds_by_size
    {
    public:
        
        using draw_pass_args_t = draw_pass::msg;
        
    public:
        
        draw_pass_args_t modify(draw_pass_args_t);
        
        static std::shared_ptr<replace_texture_2ds_by_size> make(std::map<int, std::shared_ptr<igpu::texture_2d>>);
        
    private:
        
        replace_texture_2ds_by_size(std::map<int, std::shared_ptr<igpu::texture_2d>>);
        
    private:
        using program_instance_t = std::shared_ptr<program_instance>;
        
        std::map<program_instance_t, program_instance_t> _replaced_program_instances;
        std::map<int, std::shared_ptr<igpu::texture_2d>> _replacements;
    };
}
