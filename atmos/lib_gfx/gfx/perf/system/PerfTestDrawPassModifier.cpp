
#include "gfx/perf/context/perf_test_draw_pass_modifier.h"

#include "framework/logging/log.h"

#include "gfx/mesh/geometry.h"
#include "gfx/perf/mesh/reduced_polycount_geometry.h"
#include "gfx/perf/texture/replace_texture_2ds_by_size.h"

using namespace gfx

perf_test_draw_pass_modifier::stage_binding_modifier_t perf_test_draw_pass_modifier::make_stage_binding_modifier(
    const perf_test_config& config,
    const frustum& view_frustum,
    std::map<int, std::shared_ptr<igpu::texture_2d>> replacement_textures)
{
    using draw_pass_modifier_t = std::function<draw_pass_args_t(draw_pass_args_t)>;
    
    std::vector<draw_pass_modifier_t> modifiers;
    
    // add a modifier to reduce the number of state changes within batching systems
    if(auto reduce_batching_system_state_changes = replace_texture_2ds_by_size::make(std::move(replacement_textures)))
    {
        modifiers.emplace_back([reduce_batching_system_state_changes](draw_pass_args_t msg){
            
            return reduce_batching_system_state_changes->modify(msg);
        });
    }
    
    // add a modifier to reduce the number of state changes within batching systems
    if(auto reduce_batching_system_state_changes = reduce_batching_system_state_changes::make(config.batching_config, view_frustum))
    {
        modifiers.emplace_back([reduce_batching_system_state_changes](draw_pass_args_t msg){
            
            return reduce_batching_system_state_changes->modify(msg);
        });
    }
    
    // add modifier to reduce the scene poly count
    if(should_make_modifier_for_scale(config.polycount_scale, "polycount"))
    {
        using geometry_t = std::shared_ptr<geometry>;
        using map_t = std::map<geometry_t, geometry_t>;
        
        auto mapping = std::make_shared<map_t>();
        
        float polycount_scale = config.polycount_scale;
        modifiers.emplace_back([polycount_scale, mapping](draw_pass_args_t msg){
            
            // avoid creating a larger number of geometries than the scene originally contained
            auto& geometry = (*mapping)[msg.binding_args.geometry];
            
            if(!geometry)
            {
				geometry = std::shared_ptr<geometry>(new reduced_polycount_geometry(polycount_scale, msg.binding_args.geometry));
            }
            
			msg.binding_args.geometry = geometry;
            
            return msg;
        });
    }
    
    if(!modifiers.empty())
    {
        return [modifiers](draw_pass_args_t msg){
            
            for(const auto& modifier : modifiers)
            {
				msg = modifier(msg);
            }
            
            return msg.binding_args;
        };
    }
    
    return nullptr;
}

bool perf_test_draw_pass_modifier::should_make_modifier_for_scale(float scale, const std::string& scale_type)
{
    if(scale < 0 || scale > 1)
    {
		LOG_CONTEXT(INFORM, "recieved scale(%f, %s) outsize 0 - 1", scale, scale_type.c_str() );
        return false;
    }
    
    // should make modifier if scale is not equal to 1
    return scale != 1;
}
