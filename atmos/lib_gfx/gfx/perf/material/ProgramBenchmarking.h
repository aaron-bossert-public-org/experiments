
#pragma once


#include "gfx/perf/context/geometry_debugger.h"

namespace igpu
{
    class program;
}

namespace gfx
{
    class draw_pass_modifier;
    
    class program_benchmarking : public geometry_debugger
    {
        
    public:
        
        static std::shared_ptr<program_benchmarking> make(
            bool coarse,
            int frames_per_step,
            const std::shared_ptr<igpu::program>&,
            const geometry_debugger::callback_t&);
        
    private:
        
        program_benchmarking(bool coarse,
                            int frames_per_step,
                            const std::shared_ptr<igpu::program>&,
                            const geometry_debugger::callback_t&);
        
        steps_t make_steps(draw_pass_modifier&) override;
        
    private:
        
        bool _coarse = false;
        
        std::shared_ptr<igpu::program> _replacement_program;
        
        int _frames_per_step = 0;
    };
}
