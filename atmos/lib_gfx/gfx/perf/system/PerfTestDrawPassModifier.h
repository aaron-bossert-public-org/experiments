
#pragma once

#include <functional>
#include <gfx/perf/context/perf_test_config.h>
#include <gfx/context/draw_pass_bundle.h>

namespace igpu
{
    class program;
}

namespace gfx
{
    class perf_test_draw_pass_modifier
    {
    public:
        
        using draw_pass_t = draw_pass;
        
        using binding_args_t = scene_stage_binding::msg;
        
        using draw_pass_args_t = draw_pass_t::msg;
        
        using stage_binding_modifier_t = std::function<binding_args_t(const draw_pass_args_t&)>;
        
        static stage_binding_modifier_t make_stage_binding_modifier(const perf_test_config&,
                                                                 const frustum& view_frustum,
                                                                 std::map<int, std::shared_ptr<igpu::texture_2d>> replacement_textures);
        
        static bool should_make_modifier_for_scale(float scale, const std::string& scale_type);
    };
}
