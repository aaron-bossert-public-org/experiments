#include "gfx/scene/scene.h"

#include "igpu/utility/markers.h"
#include "gfx/light/light_manager.h"
#include "gfx/material/scene_parameters_manager.h"
#include "gfx/scene/clear_stage.h"
#include "gfx/scene/scene_stage.h"
#include "gfx/scene/view_state.h"

#include <zna/source/common/utility/stack_timer.h>

using namespace gfx

void scene::render(float delta_seconds)
{
    _dispatch_time.reset();
    auto dispatch_scope = _dispatch_time.time_scope();
    STACK_TIMER("scene::render")
    ATMOS_MARK_FUNCTION;
    
    const auto& viewport = _view_state->viewport();
    
    
    _context.begin_scene();
    _context.set_viewport(_view_state->viewport());
    _context.set_back_buffer(_back_buffer.get());
    
    _clear_stage->render(*_scene_parameter_manager, viewport);
    
    if(!scene_is_empty)
    {
        _light_manager->update_scene_parameters(_scene_parameter_manager);
        
        _scene_parameter_manager->time(std::fmodf(_scene_parameter_manager->time() + delta_seconds, 60.0f * 60.0f * 2.0f));
        _scene_parameter_manager->eye_forward(_view_state->eye_forward());
        _scene_parameter_manager->eye(_view_state->eye());
        
        _scene_parameter_manager->view_matrix(_view_state->view_matrix());
        _scene_parameter_manager->proj_matrix(_view_state->proj_matrix());
        _scene_parameter_manager->view_proj_matrix(_view_state->view_proj_matrix());
        
        for(const auto& pair : _stages)
        {
            igpu::scope_marker _ (pair.second->msg().name());

			pair.second->render(*_scene_parameter_manager, viewport);
        }
    }
    
    _context.end_scene();
}

const std::shared_ptr<clear_stage>& scene::clear_stage() const
{
    return _clear_stage;
}

const std::shared_ptr<scene_stage>& scene::stage(size_t index)
{
    if(index < _stages.size())
    {
        return _stages[index];
    }
    
    static std::shared_ptr<scene_stage> s_stage;
    
    return s_stage;
}

const scene::stages_t& scene::stages() const
{
    return _stages;
}

const view_state& scene::view_state() const
{
    return *_view_state;
}

const std::shared_ptr<scene_parameters_manager>& scene::scene_parameter_manager() const
{
    return _scene_parameter_manager;
}

std::shared_ptr<scene> scene::make(
    std::string name,
    const std::shared_ptr<clear_stage>& clear_stage,
    stages_t stages,
    const std::shared_ptr<igpu::back_buffer>& back_buffer,
    const std::shared_ptr<scene_parameters_manager>& scene_parameter_manager,
    const std::shared_ptr<light_manager>& light_manager,
    const std::shared_ptr<view_state>& view_state)
{
    if(name.empty())
    {
        LOG_CONTEXT( CRITICAL, "name.empty()" );
    }
    else if(!clear_stage)
    {
        LOG_CONTEXT( CRITICAL, "clear_stage is null" );
    }
    else if(stages.empty())
    {
        LOG_CONTEXT( CRITICAL, "stages()" );
    }
    else if(!back_buffer)
    {
        LOG_CONTEXT( CRITICAL, "back_buffer is null" );
    }
    else if(!scene_parameter_manager)
    {
        LOG_CONTEXT( CRITICAL, "scene_parameter_manager is null" );
    }
    else if(!light_manager)
    {
        LOG_CONTEXT( CRITICAL, "light_manager is null" );
    }
    else if(!view_state)
    {
        LOG_CONTEXT( CRITICAL, "view_state is null" );
    }
    else
    {
		auto scene = std::shared_ptr<scene>(
			new scene(
				std::move(name),
				clear_stage,
				std::move(stages),
				back_buffer,
				scene_parameter_manager,
				light_manager,
				view_state));
        
        return scene;
    }
    
    return nullptr;
}


scene::scene(
	std::string name,
	const std::shared_ptr<clear_stage>& clear_stage,
	stages_t stages,
	const std::shared_ptr<igpu::back_buffer>& back_buffer,
	const std::shared_ptr<scene_parameters_manager>& scene_parameter_manager,
	const std::shared_ptr<light_manager>& light_manager,
	const std::shared_ptr<view_state>& view_state)
	: _name(std::move(name))
	, _clear_stage(clear_stage)
	, _stages(std::move(stages))
	, _back_buffer(back_buffer)
	, _scene_parameter_manager(scene_parameter_manager)
	, _light_manager(light_manager)
	, _view_state(view_state)
	, _draw_time(perf::category::DISPATCH_TIME, "Scene dispatch time")
{
}
