
#pragma once

#include "igpu/material/primitive.h"

#define SCENE_PARAMETERS_TUP(OP)      \
OP(view_matrix     ,glm::mat4x4)\
OP(proj_matrix     ,glm::mat4x4)\
OP(view_proj_matrix ,glm::mat4x4)\
OP(ortho_matrix    ,glm::mat4x4)\
OP(color_uniform   ,glm::vec4  )\
OP(eye            ,glm::vec3  )\
OP(eye_forward     ,glm::vec3  )\
OP(time           ,float    )\
// end tup


namespace igpu
{
    class program;
}

namespace gfx
{
    class parameter_block;
    class scene_program_binding;
    
    class scene_parameters_manager : public std::enable_shared_from_this<scene_parameters_manager>
    {
    public:
        
        using scene_program_binding_map_t = std::unordered_map<igpu::program*, std::weak_ptr<scene_program_binding>>;
        
        void set_scene_parameters(const std::shared_ptr<parameter_block>&);
        
        
#define GETTERS(param, type) type const& param() { return _##param; }
#define SETTERS(param, type) void param(const type & val) { _##param = val; set(#param, val); }
        SCENE_PARAMETERS_TUP(GETTERS)
        SCENE_PARAMETERS_TUP(SETTERS)
#undef GETTERS
#undef SETTERS
        
        template <typename T>
        void set(const std::string& name, const T& t);
        
        const igpu::primitive& get(const std::string& name);
        
        const std::unordered_map<std::string, igpu::primitive>& scene_parameters() const;
        
        std::shared_ptr<scene_program_binding> scene_program_binding(const std::shared_ptr<igpu::program>&);
        
        void enable_scene_parameter_updates(bool);
        
        static std::shared_ptr<scene_parameters_manager> make();

    private:
        
        void _set(const std::string& name, const igpu::primitive& primitive);
        
        scene_parameters_manager();
        
    private:
        
#define MEMBERS(param, type) type _##param = type();
        SCENE_PARAMETERS_TUP(MEMBERS)
#undef MEMBERS
        bool _enable_scene_parameter_updates = true;
        
        std::unordered_map<std::string, igpu::primitive> _scene_parameters;
        
        scene_program_binding_map_t _scene_program_binding_map;
    };
    
    template <typename T>
    void scene_parameters_manager::set(const std::string& name, const T& t)
    {
        igpu::primitive val = t;
        val.hash(igpu::primitive::make_hash(val));
        
        _set(name, val);
    }
}



