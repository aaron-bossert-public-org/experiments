
#pragma once

#include <igpu/material/program_binding_id.h>
#include <gfx/material/parameter_block.h>

namespace igpu
{
    class primitive;
    class parameter;
    class program;
    class render_states;
}

namespace gfx
{
    class parameter_block;
    class parameter_block_bindings;
    class scene_program_binding;
    
    // rename to material -- see rebind parameters
	class program_instance
	{
    public:
        
        struct binding
        {
            int usage_cost = 0;
            parameter_binding parameter_binding;
        };
        
    public:
        
        using bindings_t = std::vector<parameter_binding>;
        
    public:
        
        void begin() const;
        
        void end() const;
        
        void meld_parameters(parameter_block_bindings& mutable_block_bindings, const igpu::program&);
       
        const std::string& name() const { return _name; }
        
        const igpu::program& program() const;
        
        const igpu::render_states& render_states() const;
        
	public:
        
        static bool compare_by_parameters(const program_instance* lhs, const program_instance* rhs);
        
    public:
        
        static std::unique_ptr<program_instance> make(const std::string&                        name,
                                                       const std::vector<std::string>&           types,
                                                       const std::shared_ptr<igpu::program>&      program,
                                                       const std::shared_ptr<igpu::render_states>& render_states,
                                                       const std::shared_ptr<parameter_block>&    parameter_block);
        
    private:
        
        
        program_instance(const std::string&                        name,
                        const std::vector<std::string>&           types,
                        const std::shared_ptr<igpu::program>&      program,
                        const std::shared_ptr<igpu::render_states>& render_states,
                        const std::shared_ptr<parameter_block>&	  parameter_block);
        
    private:
        
        const std::string _name;
        
        const std::shared_ptr<igpu::program> _program;
        
        const std::shared_ptr<igpu::render_states> _render_states;
        
        const std::shared_ptr<parameter_block> _parameter_block;
        
        mutable bool _active = false;
        
        mutable bool _init_program_parameters = false;
        
        mutable igpu::program_binding_id _program_binding_id = (size_t)-1;
        
        mutable bindings_t _bindings;
        
        mutable std::vector<const binding*> _bound_parameter_table;
        
        mutable std::vector<const igpu::parameter*> _new_block_parameters;
        
        mutable std::vector<const igpu::parameter*> _old_block_parameters;
	};
}


