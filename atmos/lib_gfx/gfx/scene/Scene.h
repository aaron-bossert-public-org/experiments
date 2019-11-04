
#pragma once

#include "gfx/scene/view_state.h"

namespace igpu
{
    class back_buffer;
    class program;
}

namespace gfx
{
    class clear_stage;
    class light_manager;
    class scene_parameters_manager;
    class scene_stage;
    
    class scene
    {
    public:
        
        using stage_t = std::shared_ptr<scene_stage>;
        
        using stages_t = std::vector<stage_t>;
        
    public:
        
        void render(float delta_seconds);
        
        const std::shared_ptr<clear_stage>& clear_stage() const;
        
        template <typename T> std::shared_ptr<T> stage(size_t index);
        
        const std::shared_ptr<scene_stage>& stage(size_t index);
        
        const stages_t& stages() const;
        
        void view_state(const view_state&);
        
        const view_state& view_state() const;
        
        const std::shared_ptr<scene_parameters_manager>& scene_parameter_manager() const;
                
        static std::shared_ptr<scene> make(
          std::string,
          const std::shared_ptr<clear_stage>&,
          stages_t,
          const std::shared_ptr<igpu::back_buffer>&,
          const std::shared_ptr<scene_parameters_manager>&,
          const std::shared_ptr<light_manager>&,
          const std::shared_ptr<view_state>&);
    private:
        
        scene(
          std::string,
          const std::shared_ptr<clear_stage>&,
          stages_t,
          const std::shared_ptr<igpu::back_buffer>&,
          const std::shared_ptr<scene_parameters_manager>&,
          const std::shared_ptr<light_manager>&,
          const std::shared_ptr<view_state>&);
        
    private:
        
        std::string _name;
        
        const std::shared_ptr<clear_stage> _clear_stage;
        
        std::shared_ptr<igpu::back_buffer> _back_buffer;
        
        std::shared_ptr<scene_parameters_manager> _scene_parameter_manager;
        
        std::shared_ptr<light_manager> _light_manager;
        
        stages_t _stages;
        
        const std::shared_ptr<view_state> _view_state;

        perf::time_metric _dispatch_time;
    };
    
    template <typename T> std::shared_ptr<T> scene::stage(size_t index)
    {
        if (auto stage_base = this->stage(index))
        {
            auto upcast_stage = std::dynamic_pointer_cast<T>(stage_base);
            
            ASSERT_CONTEXT(upcast_stage);
            
            return upcast_stage;
        }
        
        return nullptr;
    }
}
