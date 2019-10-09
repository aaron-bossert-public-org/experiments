
#pragma once

#include <functional>

#include <gfx/scene/scene_stage_binding.h>
#include <gfx/context/draw_pass_bundle.h>

namespace igpu
{
    class program;
}

namespace gfx
{
    class draw_pass_modifier
    {
    public:
        using draw_pass_t = draw_pass;
        
        using binding_args_t = scene_stage_binding::msg;
        
        using draw_pass_args_t = draw_pass_t::msg;
        
        using draw_passes_t = std::map<const draw_pass_t*, std::weak_ptr<draw_pass_t>>;
        
        using stage_binding_modifier_t = std::function<binding_args_t(const draw_pass_args_t&)>;
        
        using enabled_condition_t = std::function<bool(const draw_pass_args_t&)>;
        
    public:
        
        void clear();
        
        const draw_passes_t& draw_passes() const;
        
        void stage_binding_modifier(const stage_binding_modifier_t&);
        
        const stage_binding_modifier_t& stage_binding_modifier() const;
        
        void enabled_condition(const enabled_condition_t&);
        
        const enabled_condition_t& enabled_condition() const;
        
        static std::shared_ptr<draw_pass_t> make_draw_pass(const std::shared_ptr<draw_pass_modifier>&, const draw_pass_t::msg&);
        
        static std::unique_ptr<draw_pass_modifier> make();
        
    private:
        
        void enforce_stage_binding_modifier(draw_pass_t&);
        
        void enforce_enabled_condition(draw_pass_t&);
        
        draw_pass_modifier() = default;
        
    private:
        
        draw_passes_t _draw_passes;
        
		enabled_condition_t _enabled_condition;
        
        stage_binding_modifier_t _stage_binding_modifier;
    };
}

