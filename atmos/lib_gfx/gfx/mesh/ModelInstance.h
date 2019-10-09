
#pragma once

#include <gfx/material/technique.h>
#include <gfx/context/draw_pass_bundle.h>

namespace gfx
{
    class model_meshes;
    class model_state;
    class draw_pass_bundle;
	
    class model_instance
    {
        std::vector<std::shared_ptr<draw_pass_bundle>> _draw_pass_bundles;
        
        std::shared_ptr<model_meshes> _model_meshes;
        
        std::shared_ptr<model_state> _model_state;
        
        model_instance(const std::shared_ptr<scene>&);
        
    public:
        
        static std::unique_ptr<model_instance> make(const std::shared_ptr<scene>&);
        
        void set_model_meshes (std::shared_ptr<draw_pass_modifier>,
                             std::shared_ptr<model_meshes>,
                             technique_sets_t);
        
        const std::shared_ptr<model_state>& model_state() const;
        
        void add_draw_passes(const technique_sets_t&);
        
        void remove_draw_passes(const technique_sets_t&);
        
        const std::shared_ptr<model_meshes>& model_meshes() const;
        
        const std::vector<std::shared_ptr<draw_pass_bundle>>& draw_pass_bundles() const;
	};
}
