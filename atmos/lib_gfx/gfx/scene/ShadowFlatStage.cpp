
#include "gfx/scene/shadow_flat_stage.h"

#include "gfx/batching/batching_system.h"
#include "gfx/material/scene_parameters_manager.h"
#include "gfx/mesh/mesh_state.h"
#include "gfx/mesh/model_state.h"

using namespace gfx

void shadow_flat_stage::on_render()
{
    frustum frustum = scene_parameters_manager()->view_proj_matrix();
    
    auto light_direction_handle = scene_parameters_manager()->get("light_direction");
    
    glm::vec3 light_direction =
    light_direction_handle
    ? math::normalize(light_direction_handle.get_vec3())
    : glm::vec3(0, 1, 0);
    
    if(signbit(light_direction.y)) light_direction = -light_direction;
    
    // calculate these outside of visibility test for performance
    float sphere_radius_scale = 1.f / light_direction.y;
    glm::vec3 sphere_offset_slope = -light_direction / light_direction.y;
    
    
    batching_system()->render([frustum, sphere_radius_scale, sphere_offset_slope](const scene_stage_binding& binding){
        
        auto translation = math::translation(binding.msg().model_state->model_matrix());
        auto visibility_sphere = binding.msg().mesh_state->visibility_sphere();
        
        // the visibily test uses the visibility_sphere as it is projected
        // onto the ground plane of the batchable, just as the shadow itself is done.
        visibility_sphere.radius *= sphere_radius_scale;
        visibility_sphere.center = visibility_sphere.center + sphere_offset_slope * (visibility_sphere.center.y - translation.y);
        
        
        // return visibility of a given stage binding
        return math::visibility_test(frustum, visibility_sphere);
    });
}
