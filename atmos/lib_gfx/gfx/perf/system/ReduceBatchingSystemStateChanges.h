
#pragma once

#include <functional>
#include <gfx/batching/batching_system_visitors.h>
#include <gfx/context/draw_pass_bundle.h>

namespace gfx
{
    class scene_stage;
    
    template <typename T>
    class batch_reducer;
    
    struct perf_test_batching_config
    {
        float program_count_scale = 1.0;
        float material_texture_scale = 1.0;
        float material_parameter_scale = 1.0;
        float vertex_buffer_count_scale = 1.0;
        float draw_count_scale = 1.0;
    };
    
    class reduce_batching_system_state_changes
    {
    public:
        
        using draw_pass_args_t = draw_pass::msg;
        
    public:
        
        draw_pass_args_t modify(draw_pass_args_t);
        
        static std::shared_ptr<reduce_batching_system_state_changes> make(const perf_test_batching_config&, const frustum& view_frustum);
        
    private:
        
        reduce_batching_system_state_changes(const perf_test_batching_config&, const frustum& view_frustum);
        
    private:
        
        frustum _view_frustum;
        
        const perf_test_batching_config _config;
        
        std::map<size_t, std::shared_ptr<batch_reducer<batches>>> _scene_stage_reducers;
    };
}
