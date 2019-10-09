
#pragma once

#include <igpu/material/program.h>
#include <gfx/material/technique.h>
#include <gfx/scene/scene_stage_binding.h>

namespace gfx
{
    class geometry;
    class mesh_state;
    class model_state;
    class draw_pass_modifier;
    class scene;
    class scene_stage;
    
    class draw_pass
    {
    public:
        
        struct msg
        {
            std::weak_ptr<draw_pass_modifier> draw_pass_modifier;
            std::weak_ptr<scene> weak_scene;
            technique technique;
            scene_stage_binding::msg binding_args;
        };
        
        draw_pass(const msg&);
        
        void batched(bool);
        
        const msg& original_args() const;
        
        const std::weak_ptr<scene_stage_binding>& weak_binding() const;
        
        bool enable() const;
        
        void enable(bool);
        
        const scene_stage_binding::msg& binding_args() const;
        
        void binding_args(const scene_stage_binding::msg&);
        
        void scene(const std::shared_ptr<scene>& scene);
        
    private:
        
        bool _enable = true;
        scene_stage_binding::msg _binding_args;
		msg _original_args;
        std::weak_ptr<scene_stage_binding> _weak_binding;
        
        draw_pass (const draw_pass&) = delete;
        draw_pass& operator= (const draw_pass&) = delete;
    };
    
    class draw_pass_bundle
    {
    public:
        
        struct msg
        {
            std::shared_ptr<geometry> geometry;
            std::weak_ptr<draw_pass_modifier> draw_pass_modifier;
            std::vector<technique> techniques;
        };
        
        using draw_passes_t = std::vector<std::shared_ptr<draw_pass>>;
        
        draw_pass_bundle(const msg&, std::shared_ptr<model_state>);
        
    public:
        
        static std::shared_ptr<draw_pass_bundle> make(const msg&, std::shared_ptr<model_state>);
        
        virtual ~draw_pass_bundle();
        
        void add_pass(const technique&);

        void remove_pass(const technique&);
        
        void remove_passes();
        
        const std::shared_ptr<geometry>& geometry() const;
        
        const std::shared_ptr<mesh_state>& mesh_state() const;
        
        const std::shared_ptr<model_state>& model_state() const;
        
        int bind_count() const;
        
        const draw_passes_t& draw_passes() const;
        
        draw_passes_t& draw_passes();
        
    protected:
        
        std::shared_ptr<geometry> _geometry;
        
        std::shared_ptr<mesh_state> _mesh_state;
        
        std::shared_ptr<model_state> _model_state;
        
        draw_passes_t _draw_passes;
        
        std::weak_ptr<draw_pass_modifier> _draw_pass_modifier;
        
        draw_pass_bundle (const draw_pass_bundle&) = delete;
		draw_pass_bundle& operator= (const draw_pass_bundle&) = delete;
    };
}


