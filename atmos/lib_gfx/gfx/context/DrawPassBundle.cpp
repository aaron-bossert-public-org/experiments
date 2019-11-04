
#include "gfx/context/draw_pass_bundle.h"

#include "framework/logging/log.h"

#include "gfx/material/program_instance.h"
#include "gfx/material/technique.h"
#include "gfx/mesh/geometry.h"
#include "gfx/mesh/mesh_state.h"
#include "gfx/mesh/model_state.h"
#include "gfx/perf/context/draw_pass_modifier.h"
#include "gfx/scene/scene.h"
#include "gfx/scene/scene_stage.h"

using namespace gfx;

draw_pass::draw_pass(const msg& msg)
: _binding_args(msg.binding_args)
, _original_args(msg)
{
}

void draw_pass::batched(bool batched)
{
    if(!_binding_args.geometry)
    {
        return;
    }
    
    auto binding = _weak_binding.lock();
    
    if(batched != (bool)binding)
    {
        if(batched)
        {
            if(_enable)
            {
                auto scene = _original_args.weak_scene.lock();
                size_t scene_stage_index = _original_args.technique.scene_stage_index();
                const auto& scene_stage = scene->stage(scene_stage_index);
                ASSERT_CONTEXT(scene_stage);
                
                if(scene_stage)
                {
                	_weak_binding = scene_stage->make_binding(_binding_args, scene);
                }
            }
        }
        else
        {
            binding->unbind();
        }
    }
}

const draw_pass::msg& draw_pass::original_args() const
{
    return _original_args;
}

const std::weak_ptr<scene_stage_binding>& draw_pass::weak_binding() const
{
    return _weak_binding;
}

bool draw_pass::enable() const
{
    return _enable;
}

void draw_pass::enable(bool enable)
{
    _enable = enable;
    
    bool should_bind = _binding_args.model_state->batched() && _enable;
    
    if(should_bind != (bool)_weak_binding.lock())
    {
        batched(should_bind);
    }
}

const scene_stage_binding::msg& draw_pass::binding_args() const
{
    return _binding_args;
}

void draw_pass::scene(const std::shared_ptr<scene>& scene)
{
    if(!scene)
    {
        LOG_CONTEXT( CRITICAL, "scene is null");
    }
    else
    {
        _original_args.weak_scene = scene;
        
        // currently bound, unbind and rebind to hook into the to the new scene stage
        if(nullptr != _weak_binding.lock())
        {
            batched(false);
            batched(true);
        }
    }
}

void draw_pass::binding_args(const scene_stage_binding::msg& binding_args)
{
    if(_binding_args != binding_args)
    {
        _binding_args = binding_args;
        
        // currently bound, unbind and rebind to hook into the to the new scene stage
        if(nullptr != _weak_binding.lock())
        {
            batched(false);
            batched(true);
        }
    }
}

draw_pass_bundle::draw_pass_bundle(const msg& msg, std::shared_ptr<model_state> model_state)
: _geometry(msg.geometry)
, _draw_pass_modifier (msg.draw_pass_modifier)
, _mesh_state( mesh_state::make() )
, _model_state(std::move(model_state))
{
    for (auto& technique : msg.techniques)
    {
        add_pass(technique);
    }
}

std::shared_ptr<draw_pass_bundle> draw_pass_bundle::make(const msg& msg, std::shared_ptr<model_state> model_state)
{
    return std::shared_ptr<draw_pass_bundle>(new draw_pass_bundle(msg, std::move(model_state)));
}

draw_pass_bundle::~draw_pass_bundle()
{
    for(auto& draw_pass : _draw_passes)
    {
        draw_pass->batched(false);
    }
}

void draw_pass_bundle::add_pass(const technique& technique)
{
#if ATMOS_DEBUG
    for (auto& draw_pass : _draw_passes)
    {
        if (draw_pass->original_args().technique.scene_stage_index() == technique.scene_stage_index())
        {
            LOG_CONTEXT( CRITICAL, "more than one technique added for stage(%d : %s), you should call remove_pass for this stage",
                      (int)technique.scene_stage_index(),
                      technique.scene_stage_name().c_str());
        }
    }
#endif
    
    auto scene = _model_state->scene();
    
    if(!scene)
    {
        LOG_CONTEXT( CRITICAL, "scene is null");
    }
    else
    {
        
        draw_pass::msg msg = {
            _draw_pass_modifier,
            scene,
            technique,
            {
                technique.program_instance()->program(),
                technique.program_instance(),
                _geometry,
                _mesh_state,
                _model_state,
            },
        };
        
        std::shared_ptr<draw_pass> draw_pass;
        
        // draw_pass_modifier can be used to alter the way objects are presented to the renderer for diagnostic purposes
        if(auto draw_pass_modifier = _draw_pass_modifier.lock())
        {
            draw_pass = draw_pass_modifier->make_draw_pass(draw_pass_modifier, msg);
        }
        
        if(!draw_pass)
        {
            draw_pass = std::make_shared<draw_pass>(msg);
        }
        
        draw_pass->batched(_model_state->batched());
        _draw_passes.push_back(draw_pass);
    }
}

void draw_pass_bundle::remove_pass(const technique& technique)
{
    for (auto& draw_pass : _draw_passes)
    {
        if (draw_pass->original_args().technique.scene_stage_index() == technique.scene_stage_index())
        {
            draw_pass->batched(false);
            
            draw_pass = _draw_passes.back();
            _draw_passes.pop_back();
            break;
        }
    }
}

void draw_pass_bundle::remove_passes()
{
    for (auto& draw_pass : _draw_passes)
    {
        draw_pass->batched(false);
    }
    
    _draw_passes.clear();
}

const std::shared_ptr<geometry>& draw_pass_bundle::geometry() const
{
    return _geometry;
}

const std::shared_ptr<mesh_state>& draw_pass_bundle::mesh_state() const
{
    return _mesh_state;
}

const std::shared_ptr<model_state>& draw_pass_bundle::model_state() const
{
    return _model_state;
}

int draw_pass_bundle::bind_count() const
{
    int bind_count = 0;
    
    for (auto& draw_pass : _draw_passes)
    {
        if (draw_pass->weak_binding().lock())
        {
            ++bind_count;
        }
    }
    
    return bind_count;
}

const draw_pass_bundle::draw_passes_t& draw_pass_bundle::draw_passes() const
{
    return _draw_passes;
}

draw_pass_bundle::draw_passes_t& draw_pass_bundle::draw_passes()
{
    return _draw_passes;
}
