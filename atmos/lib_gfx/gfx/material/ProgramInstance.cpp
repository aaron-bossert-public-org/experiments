
#include <gfx/material/program_instance.h>

#include <framework/logging/log.h>
#include <igpu/material/parameter.h>
#include <igpu/material/program.h>
#include <gfx/material/parameter_block.h>
#include <gfx/material/parameter_block_bindings.h>
#include <gfx/material/scene_program_binding.h>

using namespace gfx;

// use this to create/store hashes for primitives
namespace std
{
	template<typename T>
	struct hash<std::vector<T>>
	{
		size_t operator()(const std::vector<T>& v)
		{
			size_t seed = 0;
			std::hash<T> hasher;

			for (const auto& e : v)
			{
				hash_combine(seed, hasher(e));
			}

			return seed;
		}
	};
}

namespace
{
    using binding_t = program_instance::binding;
    
    program_instance::bindings_t generate_parameter_bindings(const std::shared_ptr<parameter_block>& parameter_block,
                                                          const std::shared_ptr<igpu::program>& program)
    {
        program_instance::bindings_t parameter_bindings;
        
        const auto& primitives = parameter_block->primitives();
        
        for(const auto& handle : parameter_block->handles())
        {
            const auto& name = handle.first;
            size_t index = handle.second;
            
            if(const auto* parameter = program->lookup(name))
            {
                parameter_bindings.emplace_back( .resize(parameter_bindings.size() + 1);
                binding_t& binding = parameter_bindings.back();
                
                binding.parameter_binding.set(name, &primitives[index], *program);
                binding.usage_cost 
					= parameter 
					? parameter->info()._type * parameter->info()._size 
					: 0;
            }
        }
        
        // sort program instance parameters loosely based on the cost of updating a program with them

        // for batching, program instances are lexically ordered by their parameter hashes
        // the lexical position of each hash is primarily dependant on cost of updating corrisponding parameter,
        // and secondarily dependent on parameter name
        const auto expensive_params_first = [](const binding_t& lhs, const binding_t& rhs){
            
            if(lhs.usage_cost != rhs.usage_cost)
            {
                return lhs.usage_cost > rhs.usage_cost;
            }
            
            return lhs.parameter_binding.name() > rhs.parameter_binding.name();
        };
        
        std::sort(parameter_bindings.begin(), parameter_bindings.end(), expensive_params_first);
        
        return std::move(parameter_bindings);
    }
    
    std::vector<const binding_t*> bound_parameters_for_program(const program_instance::lookup_table_t& lookup_table,
                                                            const program_instance::bindings_t& bindings,
                                                            const std::shared_ptr<igpu::program>& program)
    {
        const auto& parameters = program->parameters();
        
        // parameters are defaulted to -1
        std::vector<const binding_t*> bound_parameters(parameters.size(), nullptr);
        
        for(const auto* parameter : parameters)
        {
            auto find = lookup_table.find(parameter->name());
            
            if(find != lookup_table.end())
            {
                bound_parameters[parameter->index()] = &bindings[find->second];
                ASSERT_CONTEXT(parameter->index() < parameters.size());
            }
        }
        
        return std::move(bound_parameters);
    }
}

void program_instance::begin() const
{
    ASSERT_CONTEXT((_active = !_active ) == true);
    _init_program_parameters = true;
}

void program_instance::end() const
{
    ASSERT_CONTEXT((_active = !_active ) == false);
}

void program_instance::meld_parameters(parameter_block_bindings& mutable_block_bindings, const std::shared_ptr<igpu::program>& program)
{
    ASSERT_CONTEXT(_active);
    
    rebind_parameters(program);
    
    // swap old and new parameters, the old ones will be reset to values found in the material
    std::swap(_new_block_parameters, _old_block_parameters);
    
    
    const auto& parameter_bindings = mutable_block_bindings.parameter_bindings(*program);
    const value_hash_t& block_hash = mutable_block_bindings.parameter_block().hash();
    
    // apply the parameter block to the program
    for(auto& binding : parameter_bindings)
    {
        if (auto* parameter = binding.parameter())
        {
            program->update(parameter, *binding.primitive(), true);
            _new_block_parameters.emplace_back(binding.parameter());
        }
    }
    
    if( _init_program_parameters )
    {
        _init_program_parameters = false;
        
        // reset all parameters to material values iff they were not set by the new parameter block
        // instead of testing hashes for parameer value differences
        for(auto& binding : _bindings)
        {
            const auto& parameter_binding = binding.parameter_binding;
            const auto* primitive = parameter_binding.primitive();
            const auto* parameter = parameter_binding.parameter();
            
            if(block_hash != parameter->hash())
            {
                program->update(parameter, *primitive);
            }
        }
    }
    else
    {
        // reset old block parameters to material values iff they were not set by the new parameter block.
        for(auto* old_block_parameter : _old_block_parameters)
        {
            if(block_hash != old_block_parameter->hash())
            {
                if(const auto* binding = _bound_parameter_table[old_block_parameter->index()])
                {
                    program->update(old_block_parameter, *binding->parameter_binding.primitive(), true);
                }
            }
        }
    }
    
    _old_block_parameters.clear();
}

const std::shared_ptr<igpu::program>& program_instance::program() const
{
    return _program;
}

const std::shared_ptr<igpu::render_states>& program_instance::render_states() const
{
    return _render_states;
}

const program_instance::bindings_t& program_instance::bindings() const
{
    return _bindings;
}

bool program_instance::compare_by_parameters(const program_instance* lhs, const program_instance* rhs)
{
    // dig through the parameters of lhs, returning the comparison result of tht first inequal
    // hashes of equal parameter name.
    size_t size = std::min(lhs->_bindings.size(), rhs->_bindings.size());
    
    for(size_t p = 0; p < size; ++p)
    {
        const auto& lparam_binding = lhs->_bindings[p].parameter_binding;
        const auto& rparam_binding = rhs->_bindings[p].parameter_binding;
        
        // do not assume program instances specify primitive values for the same subesets
        // parameters: compare by parameters by name then the hash of their primitives
        if(lparam_binding.name() != rparam_binding.name())
        {
            return lparam_binding.name() < rparam_binding.name();
        }
        else
        {
            const auto& l_hash = lparam_binding.primitive()->hash();
            const auto& r_hash = rparam_binding.primitive()->hash();
            
            if(l_hash != r_hash)
            {
                return l_hash < r_hash;
            }
        }
    }
    
    return lhs->_bindings.size() < rhs->_bindings.size();
}

void program_instance::rebind_parameters(const std::shared_ptr<igpu::program>& program) const
{
    igpu::program_binding_id program_binding_id = program->binding_id();
    
    if (_program_binding_id != program_binding_id)
    {
        _program_binding_id = program_binding_id;
        _bindings = generate_parameter_bindings(_parameter_block, program);
		_bound_parameter_table = bound_parameters_for_program(_bindings, program);
        _new_block_parameters.clear();
        _old_block_parameters.clear();
        _new_block_parameters.reserve( _bindings.size() );
        _old_block_parameters.reserve( _bindings.size() );
    }
}

std::unique_ptr<program_instance> program_instance::make(
    const std::string&                        name,
    const std::vector<std::string>&           types,
    const std::shared_ptr<igpu::program>&      program,
    const std::shared_ptr<igpu::render_states>& render_states,
    const std::shared_ptr<parameter_block>&    parameter_block)
{
    if(name.empty())
    {
        LOG_CONTEXT( CRITICAL, "name is empty");
    }
    else if (types.empty())
    {
        LOG_CONTEXT( CRITICAL, "types filter is empty");
    }
    else if (!program)
    {
        LOG_CONTEXT( CRITICAL, "program is null");
    }
    else if (!render_states)
    {
        LOG_CONTEXT( CRITICAL, "render_states is null");
    }
    else if (!parameter_block)
    {
        LOG_CONTEXT( CRITICAL, "parameters are null");
    }
    else
    {
        
#if ATMOS_DEBUG
        
        const auto& primitives = parameter_block->primitives();
        
        for(const auto& handle : parameter_block->handles())
        {
            const auto& name = handle.first;
            int index = handle.second;
            const auto& primitive = primitives[index];
            
            if (!primitive.hash().val())
            {
                LOG_CONTEXT( CRITICAL,
                          "parameter %s must have a hashed primitive to be used with a program instance",
                          name.c_str());
            }
        }
#endif
        
        return std::unique_ptr<program_instance>
        (new program_instance(name,
                             types,
                             program,
                             render_states,
                             parameter_block)
         );
    }
    
    return nullptr;
}

program_instance::program_instance(const std::string&                        name,
                                 const std::vector<std::string>&           types,
                                 const std::shared_ptr<igpu::program>&      program,
                                 const std::shared_ptr<igpu::render_states>& render_states,
                                 const std::shared_ptr<parameter_block>&	   parameter_block)
: _name(name)
, _types(types)
, _program(program)
, _render_states(render_states)
, _parameter_block(parameter_block)
{
    rebind_parameters(program);
}
