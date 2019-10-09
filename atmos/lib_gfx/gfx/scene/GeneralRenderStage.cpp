
#include <gfx/scene/general_render_stage.h>

#include <gfx/batching/batching_system.h>
#include <gfx/material/scene_parameters_manager.h>
#include <gfx/mesh/mesh_state.h>

using namespace gfx

general_render_stage::general_render_stage(const pb::scene_stage& msg, const std::shared_ptr<scene_parameters_manager>& scene_parameters_manager)
: scene_stage(msg)
, _scene_parameters_manager(scene_parameters_manager)
, _batching_system(batching_system::make([this] {
    
    empty(true);
}))
{
}

void general_render_stage::on_render()
{
    frustum frustum = _scene_parameters_manager->view_proj_matrix();
    
    // draw batched
    _batching_system->render(frustum);
}

scene_stage::binding_t general_render_stage::make_binding(const scene_stage_binding::msg& msg, const std::shared_ptr<scene>& scene)
{
    scene_stage::binding_t binding = _batching_system->make_binding(msg, scene);
    
    empty(_batching_system->empty());
    
    return std::move(binding);
}

const std::shared_ptr<scene_parameters_manager>& general_render_stage::scene_parameters_manager() const
{
    return _scene_parameters_manager;
}

const std::shared_ptr<batching_system>& general_render_stage::batching_system() const
{
    return _batching_system;
}
