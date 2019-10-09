
#include <gfx/perf/material/program_benchmarking.h>
#include <gfx/perf/context/draw_pass_modifier.h>
#include <gfx/material/program_instance.h>
#include <gfx/material/scene_parameters_manager.h>
#include <gfx/scene/scene_stage.h>
#include <gfx/context/draw_pass_bundle.h>

using namespace gfx;

namespace
{
    std::map<std::shared_ptr<igpu::program>, std::set<std::string>> active_program_uses(const draw_pass_modifier& draw_pass_modifier)
    {
        std::map<std::shared_ptr<igpu::program>, std::set<std::string>> active_programs;
        
        for(const auto& pair : draw_pass_modifier.draw_passes())
        {
            auto draw_pass = pair.second.lock();
            if(!draw_pass)
            {
                LOG_CONTEXT( CRITICAL, "draw_pass was not removed before it expired" );
                continue;
            }
            else if(auto program_instance = draw_pass->original_args().binding_args.program_instance)
            {
                if(auto program = draw_pass->original_args().binding_args.program)
                {
                    const auto file = xtd::path::name(program_instance->desc());
                    active_programs[program].insert(file);
                }
            }
        }
        
        return active_programs;
    }

    class program_benchmark_all : public geometry_debugger_step
    {
    public:
        
        program_benchmark_all(std::shared_ptr<igpu::program> replacement_program);
        
    private:
        
        void setup_step(draw_pass_modifier&) const override;
        
        void teardown_step(draw_pass_modifier&) const override;
        
    private:
        
        std::shared_ptr<igpu::program> _replacement_program;
    };
    
    program_benchmark_all::program_benchmark_all(std::shared_ptr<igpu::program> replacement_program)
    : geometry_debugger_step("Repace all programs with simple one")
    , _replacement_program(replacement_program)
    {
    }
    
    void program_benchmark_all::setup_step(draw_pass_modifier& draw_pass_modifier) const
    {
        std::shared_ptr<igpu::program> replacement = _replacement_program;
        
        draw_pass_modifier.stage_binding_modifier([replacement](const draw_pass::msg& msg){
            
            auto binding_args = msg.binding_args;
            
            binding_args.program = replacement;
            
            return binding_args;
        });
    }
    
    void program_benchmark_all::teardown_step(draw_pass_modifier& draw_pass_modifier) const
    {
    }
    
    //--------------------------------------------------------------------------
    //
    //
    class program_benchmark : public geometry_debugger_step
    {
    public:
        
        program_benchmark(const std::string& description,
                         std::shared_ptr<igpu::program> target_program,
                         std::shared_ptr<igpu::program> replacement_program);
        
    private:
        
        void setup_step(draw_pass_modifier&) const override;
        
        void teardown_step(draw_pass_modifier&) const override;
        
    private:
        
        std::shared_ptr<igpu::program> _target_program;
        
        std::shared_ptr<igpu::program> _replacement_program;
    };
    
    program_benchmark::program_benchmark(const std::string& description,
                                       std::shared_ptr<igpu::program> target_program,
                                       std::shared_ptr<igpu::program> replacement_program)
    : geometry_debugger_step(description)
    , _target_program(target_program)
    , _replacement_program(replacement_program)
    {
    }
    
    void program_benchmark::setup_step(draw_pass_modifier& draw_pass_modifier) const
    {
        std::shared_ptr<igpu::program> target = _target_program;
        std::shared_ptr<igpu::program> replacement = _replacement_program;
        
        draw_pass_modifier.stage_binding_modifier([target, replacement](const draw_pass::msg& msg){
            
            auto binding_args = msg.binding_args;
            
            if(binding_args.program == target)
            {
                binding_args.program = replacement;
            }
            
            return binding_args;
        });
    }
    
    void program_benchmark::teardown_step(draw_pass_modifier& draw_pass_modifier) const
    {
    }
}
    
//--------------------------------------------------------------------------
//
//
std::shared_ptr<program_benchmarking> program_benchmarking::make(
    bool coarse,
    int frames_per_step,
    const std::shared_ptr<igpu::program>& replacement_program,
    const callback_t& callback)
{
    if(!replacement_program)
    {
        LOG_CONTEXT( CRITICAL, "replacement_program is null");
    }
    else if(!frames_per_step)
    {
        LOG_CONTEXT( CRITICAL, "frames_per_step is zero");
    }
    else if(!callback)
    {
        LOG_CONTEXT( CRITICAL, "callback not defined");
    }
    else
    {
        return std::shared_ptr<program_benchmarking>(new program_benchmarking(coarse, frames_per_step, replacement_program, callback));
    }
    
    return nullptr;
}

program_benchmarking::program_benchmarking(
    bool coarse,
    int frames_per_step,
    const std::shared_ptr<igpu::program>& replacement_program,
    const callback_t& callback)
: geometry_debugger("Program_benchmarking", frames_per_step, callback)
, _coarse(coarse)
, _replacement_program (replacement_program)
, _frames_per_step (frames_per_step)
{
}

geometry_debugger::steps_t program_benchmarking::make_steps(draw_pass_modifier& draw_pass_modifier)
{
	geometry_debugger::steps_t steps;
    
    if(_replacement_program)
    {
        if(_coarse)
        {
            steps.emplace_back( std::make_shared<program_benchmark_all> (_replacement_program) );
        }
        else
        {
            for(const auto& pair : active_program_uses(draw_pass_modifier))
            {
                const auto& target_program = pair.first;
                const auto& program_instances = pair.second;
                
                std::string description;
                
                for(const std::string& program_instance : program_instances)
                {
                    description += "\n" + program_instance;
                }
                
                if(target_program)
                {
                    steps.emplace_back( std::make_shared<program_benchmark> (description, target_program, _replacement_program) );
                }
            }
        }
    }
    
    return std::move(steps);
}
