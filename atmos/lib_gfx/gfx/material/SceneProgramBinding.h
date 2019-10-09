
#pragma once

#include <igpu/material/program_binding_id.h>
#include <gfx/material/parameter_block.h>
#include <igpu/material/primitive.h>
#include <igpu/material/program_binding_id.h>

namespace igpu
{
    class parameter;
    class primitive;
    class program;
}

namespace gfx
{
    class scene_parameters_manager;
    
	class scene_program_binding
	{
    public:

        using program_t = std::shared_ptr<igpu::program>;
        
        void begin();
        
        void end();
        
        const program_t& program() const;
        
        void update(const std::string&, const igpu::primitive&);
        
        static std::unique_ptr<scene_program_binding> make(const std::weak_ptr<scene_parameters_manager>&, const program_t&);
        
    private:
        
        scene_program_binding(const std::weak_ptr<scene_parameters_manager>&, const program_t&);
        
    private:
        
        igpu::program_binding_id _program_binding_id;
        
        std::unordered_map<const igpu::parameter*, igpu::primitive> _pending_primitives;
    	
        bool _active = false;
        
        program_t _program;
        
        void* const _scene_token;
        
        std::weak_ptr<scene_parameters_manager> _scene_parameters_manager;
	};
}


