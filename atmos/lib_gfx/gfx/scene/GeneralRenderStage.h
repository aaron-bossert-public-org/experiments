
#pragma once

#include "gfx/scene/scene_stage.h"

namespace gfx
{
    class batching_system;
    class scene_parameters_manager;
    
	class general_render_stage : public scene_stage
	{
	public:
        general_render_stage(const pb::scene_stage&, const std::shared_ptr<scene_parameters_manager>&);
        
        void on_render() override;
        
        scene_stage::binding_t make_binding(const std::shared_ptr<scene>& scene) override;
        
        const std::shared_ptr<scene_parameters_manager>& scene_parameters_manager() const;
        
    protected:
        
        const std::shared_ptr<batching_system>& batching_system() const;
        
    private:
        std::shared_ptr<scene_parameters_manager> _scene_parameters_manager;
        std::shared_ptr<batching_system> _batching_system;
	};
}
