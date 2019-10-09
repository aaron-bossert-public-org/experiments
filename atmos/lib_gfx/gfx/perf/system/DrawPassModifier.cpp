
#include <gfx/perf/context/draw_pass_modifier.h>

#include <gfx/perf/context/geometry_debugger.h>
#include <gfx/material/program_instance.h>
#include <gfx/scene/scene_stage.h>
#include <gfx/context/draw_pass_bundle.h>


using namespace gfx;

void draw_pass_modifier::clear()
{
    // disable all current modifications made to geometry passes
    stage_binding_modifier(nullptr);
    enabled_condition(nullptr);
}

const draw_pass_modifier::draw_passes_t& draw_pass_modifier::draw_passes() const
{
    return _draw_passes;
}

void draw_pass_modifier::stage_binding_modifier(const stage_binding_modifier_t& stage_binding_modifier)
{
    _stage_binding_modifier = stage_binding_modifier;
    
    for(const auto& pair : _draw_passes)
    {
        auto draw_pass = pair.second.lock();
        if(!draw_pass)
        {
            LOG_CONTEXT( CRITICAL, "draw_pass was not removed before it expired");
            continue;
        }
        
        enforce_stage_binding_modifier(*draw_pass);
    }
}

const draw_pass_modifier::stage_binding_modifier_t& draw_pass_modifier::stage_binding_modifier() const
{
    return _stage_binding_modifier;
}

void draw_pass_modifier::enabled_condition(const enabled_condition_t& enabled_condition)
{
    _enabled_condition = enabled_condition;
    
    for(const auto& pair : _draw_passes)
    {
        auto draw_pass = pair.second.lock();
        if(!draw_pass)
        {
            LOG_CONTEXT( CRITICAL, "draw_pass was not removed before it expired");
            continue;
        }
        
        enforce_enabled_condition(*draw_pass);
    }
}

const draw_pass_modifier::enabled_condition_t& draw_pass_modifier::enabled_condition() const
{
    return _enabled_condition;
}

std::shared_ptr<draw_pass_modifier::draw_pass_t> draw_pass_modifier::make_draw_pass(const std::shared_ptr<draw_pass_modifier>& draw_pass_modifier, const draw_pass_t::msg& msg)
{
    struct {
        
        std::weak_ptr<draw_pass_modifier> weak_draw_pass_modifier;
        
        void operator()(draw_pass_modifier::draw_pass_t* draw_pass) const {
            
            if(auto draw_pass_modifier = weak_draw_pass_modifier.lock())
            {
            	auto& draw_passes = draw_pass_modifier->_draw_passes;
            	draw_passes.erase(draw_pass);
            }
            
            delete draw_pass;
        }
    } deleter = { draw_pass_modifier };
    
    // make a draw pass that will remove itself from the modifier when destroyed
    auto draw_pass = std::shared_ptr<draw_pass_t>(new draw_pass_t(msg), deleter);
    
    
    draw_pass_modifier->_draw_passes[draw_pass.get()] = draw_pass;
    
    draw_pass_modifier->enforce_enabled_condition(*draw_pass);
    draw_pass_modifier->enforce_stage_binding_modifier(*draw_pass);
    
    return draw_pass;
}

std::unique_ptr<draw_pass_modifier> draw_pass_modifier::make()
{
    return std::unique_ptr<draw_pass_modifier>(new draw_pass_modifier);
}

void draw_pass_modifier::enforce_stage_binding_modifier(draw_pass_t& draw_pass)
{
    if(_stage_binding_modifier)
    {
    	draw_pass.binding_args(_stage_binding_modifier(draw_pass.original_args()));
    }
    else
    {
        draw_pass.binding_args(draw_pass.original_args().binding_args);
    }
}

void draw_pass_modifier::enforce_enabled_condition(draw_pass_t& draw_pass)
{
    if(_enabled_condition)
    {
        draw_pass.enable(_enabled_condition(draw_pass.original_args()));
    }
    else
    {
        draw_pass.enable(true);
    }
}
