
#include "gfx/scene/outline_stage.h"

#include "gfx/batching/batching_system.h"
#include "gfx/mesh/mesh_state.h"
#include "gfx/material/program_instance.h"
#include "gfx/material/scene_parameters_manager.h"

using namespace gfx;

void outline_stage::on_render()
{
    frustum frustum = scene_parameters_manager()->view_proj_matrix();
    
    // draw batched
    batching_system()->render(frustum);
}
