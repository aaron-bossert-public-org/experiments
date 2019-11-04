
#pragma once

#include <functional>
#include "gfx/scene/scene_stage_binding.h"

namespace gfx
{
    class scene;
    class scene_parameters_manager;
    
    class scene_stage
	{
    public:
        
        using binding_t = std::weak_ptr<scene_stage_binding>;
        
    public:
        
        scene_stage(const pb::scene_stage&);
        
        virtual ~scene_stage() {}
        
        virtual binding_t make_binding(const scene_stage_binding::msg&, const std::shared_ptr<scene>& scene);
        
        const pb::scene_stage& msg() const;
        
        void render(scene_parameters_manager&, const utility::viewport&);
        
    protected:
        
        void empty(bool);
        
        virtual void on_render() = 0;
        
    private:
        
        const pb::scene_stage _msg;
        
        bool _empty = false;
        
		scene_stage (const scene_stage&) = delete;
		scene_stage& operator= (const scene_stage&) = delete;
	};
}
