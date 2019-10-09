
#include <gfx/material/scene_program_binding.h>

#include <framework/logging/log.h>
#include <igpu/material/program.h>
#include <gfx/material/scene_parameters_manager.h>

using namespace gfx;

void scene_program_binding::begin()
{
    ASSERT_CONTEXT(true == (_active = !_active ));
    _context.begin_program(_program.get());
    
    if(_scene_token == _program->scene_token() && _program_binding_id == _program->binding_id())
    {
        // flush pending primitives into program
        for(const auto& pair : _pending_primitives)
        {
            _program->update(pair.first, pair.second, false);
        }
    }
    else
    {
        auto scene_parameters_manager = _scene_parameters_manager.lock();
        ASSERT_CONTEXT(scene_parameters_manager);
        
        if(scene_parameters_manager)
        {
            // flush pending primitives into program
            for(const auto& pair : scene_parameters_manager->scene_parameters())
            {
                if(const auto* lookup = _program->lookup(pair.first))
                {
                    _program->update(lookup, pair.second, false);
                }
            }
            
            _program->scene_token(_scene_token);
            _program_binding_id = _program->binding_id();
        }
    }
    
    _pending_primitives.clear();
}

void scene_program_binding::update(const std::string& name, const igpu::primitive& primitive)
{
    ASSERT_CONTEXT(igpu::primitive::TYPE_UNDEFINED != primitive.get_type());
    
    if(const auto* lookup = _program->lookup(name))
    {
        if(_active)
        {
            _program->update(lookup, primitive, false);
            
        }
        else
        {
            _pending_primitives[lookup] = primitive;
        }
    }
}

void scene_program_binding::end()
{
    ASSERT_CONTEXT(false == (_active = !_active ));
    _context.end_program(_program.get());
}

const scene_program_binding::program_t& scene_program_binding::program() const
{
    return _program;
}

std::unique_ptr<scene_program_binding> scene_program_binding::make(
    const std::weak_ptr<scene_parameters_manager>& scene_parameters_manager,
    const program_t& program)
{
    if (!program)
    {
        LOG_CONTEXT( CRITICAL, "program is null");
    }
    else
    {
        return std::unique_ptr<scene_program_binding>(new scene_program_binding(scene_parameters_manager, program));
    }
    
    return nullptr;
}

scene_program_binding::scene_program_binding(const std::weak_ptr<scene_parameters_manager>& scene_parameters_manager,
                                         const program_t& program)
: _program(program)
, _scene_token(scene_parameters_manager.lock().get())
, _scene_parameters_manager(scene_parameters_manager)
{
}

