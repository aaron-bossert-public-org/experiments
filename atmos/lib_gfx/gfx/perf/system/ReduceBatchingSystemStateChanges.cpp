
#include <gfx/perf/context/reduce_batching_system_state_changes.h>

#include <igpu/material/parameter.h>

#include <gfx/material/program_instance.h>
#include <gfx/mesh/geometry.h>
#include <gfx/mesh/mesh_state.h>
#include <gfx/perf/context/perf_test_draw_pass_modifier.h>

using namespace gfx

using binding_args_t = scene_stage_binding::msg;

// use to test perf impact of potential scene content optimizations.
//
// wrap batches/program_batch/etc... to reduce state changes
// with in each node of the batching system.
//
// the reduction logic is specific to each layer.
//
// mimic optimizations by modifying the binding msg of each geometry pass of
// each geometry instance.
//
template<typename batch_t>
class batch_reducer : public batch_t
{
public:
    
    using batch_t::batch_t;
    
    binding_args_t modify(const perf_test_batching_config&, binding_args_t msg) { return msg; }
};

// given a target reduction scale, reduce unique incoming items to a smaller
// set of outgoing unique items
template<typename item_t, class compare_t = std::less<item_t>>
struct reducer
{
    // return true if item was mapped to a different value
    item_t reduce(const item_t& item, float reduction_scale)
    {
        auto find = _item_map.find(item);
        if(find != _item_map.end())
        {
            // be consistant if this value has previously been encountered
            return find->second;
        }
        else
        {
            bool will_reduce = true;
            
            if(_item_map.empty())
            {
                // always have at least one unique item
                will_reduce = false;
            }
            else
            {
                float possible_reduction_scale
                = (1.f + (float)_final_unique_items.size())
                / (1.f + (float)_item_map.size());
                
                will_reduce = reduction_scale < possible_reduction_scale;
            }
                
            if(will_reduce)
            {
                // too many unique items: map item onto an existing value
                _item_map[item] = *_final_unique_items.begin();
            }
            else
            {
                // not too many unique items: keep the item's value as it is
                _item_map[item] = item;
                _final_unique_items.insert(item);
            }
        }
        
        return _item_map[item];
    }
    
private:
    
    std::set<item_t, compare_t> _final_unique_items;
    std::map<item_t, item_t, compare_t> _item_map;
};


//-------------------------------------------------------------------------
// reduce the number of child batches by decreasing number of unique material
// textures and parameters they refrence
//
template<>
class batch_reducer<batches> : public batches
{
public:
    batch_reducer() : batches([]{}) {}
    
    binding_args_t modify(const perf_test_batching_config& config, binding_args_t msg)
    {
        float program_count_scale = config.program_count_scale;
        
        if(!perf_test_draw_pass_modifier::should_make_modifier_for_scale(program_count_scale, "program"))
        {
            return msg;
        }
        
		msg.program = _reducer.reduce(msg.program, program_count_scale);
        
        return msg;
    }
    
private:
    
    reducer<std::shared_ptr<igpu::program>> _reducer;
};

//-------------------------------------------------------------------------
// reduce the number of parameter state changes by decreasing number of
// unique material textures and parameters they refrence
//
template<>
class batch_reducer<program_batch> : public program_batch
{
public:
    using program_batch::program_batch;
    
    binding_args_t modify(const perf_test_batching_config& config, binding_args_t msg)
    {
        float material_parameter_scale = config.material_parameter_scale;
        float material_texture_scale = config.material_texture_scale;

        if(!perf_test_draw_pass_modifier::should_make_modifier_for_scale(material_parameter_scale, "material parameter") &&
           !perf_test_draw_pass_modifier::should_make_modifier_for_scale(material_texture_scale, "material texture"))
        {
            return msg;
        }
        
        auto& reduced_program_instance = _reduced_program_instances[msg.program_instance];
        
        if(!reduced_program_instance)
        {
            std::vector<std::pair<parameter_id, igpu::primitive>> primitives;
            
            for(const auto& handle : msg.program->parameters())
            {
                const auto& name = handle->name();
                if(const igpu::primitive* primitive = msg.program_instance->primitive(name))
                {
                    auto* reducer = &_primitive_reducers[name];
                    float reduction_scale = material_parameter_scale;
                    
                    if(igpu::parameter_type::PARAMETER_TYPE_SAMPLER_2D == handle->info()._type)
                    {
                        reducer = &_texture_reducers[name];
                        reduction_scale = material_texture_scale;
                    }
                    
                    primitives.emplace_back(name, reducer->reduce(*primitive, reduction_scale));
                }
            }
            
            auto original = msg.program_instance;
            
            auto desc =
            "Reduced " + original->desc() +
            " texture_scale: " + std::to_string(material_texture_scale) +
            " parameter_scale: " + std::to_string(material_parameter_scale);
            
            auto parameter_block = std::make_shared<parameter_block>(primitives, true);
            
            reduced_program_instance = program_instance::make(desc, original->types(), original->program(), original->render_states(), parameter_block);
        }
        
		msg.program_instance = reduced_program_instance;
        
        return msg;
    }
    
private:
    
    struct compare_primitive
    {
        bool operator () (const igpu::primitive& l, const igpu::primitive& r) const
        {
            return l.hash().val() < r.hash().val();
        }
    };
    
    using reducer_t = reducer<igpu::primitive, compare_primitive>;
    using program_instance_t = std::shared_ptr<program_instance>;
    
    std::map<program_instance_t, program_instance_t> _reduced_program_instances;
    std::map<std::string, reducer_t> _primitive_reducers;
    std::map<std::string, reducer_t> _texture_reducers;
};

//-------------------------------------------------------------------------
// reduce the number of geometries (not draw calls)
//
template<>
class batch_reducer<material_batch> : public material_batch
{
public:
    using material_batch::material_batch;
    
    binding_args_t modify(const perf_test_batching_config& config, binding_args_t msg)
    {
        float vertex_buffer_count_scale = config.vertex_buffer_count_scale;
        
        if(!perf_test_draw_pass_modifier::should_make_modifier_for_scale(vertex_buffer_count_scale, "vertex buffer count"))
        {
            return msg;
        }
        
        auto& reduced_geometry = _reduced_geometries[msg.geometry];
        
        if(!reduced_geometry)
        {
            reduced_geometry = _reducer.reduce(msg.geometry, vertex_buffer_count_scale);
        }
        
		msg.geometry = reduced_geometry;
        
        return msg;
    }
    
private:
    
    using geometry_t = std::shared_ptr<geometry>;
    using reducer_t = reducer<geometry_t>;
    
    std::map<geometry_t, geometry_t> _reduced_geometries;
    reducer<geometry_t> _reducer;
};

//-------------------------------------------------------------------------
// reduce the number of geometries (not draw calls)
//
template<>
class batch_reducer<draw_batch> : public draw_batch
{
public:
    using draw_batch::draw_batch;
    
    binding_args_t modify(const perf_test_batching_config& config, binding_args_t msg)
    {
        float draw_count_scale = config.draw_count_scale;
        
        if(!perf_test_draw_pass_modifier::should_make_modifier_for_scale(draw_count_scale, "draw count"))
        {
            return msg;
        }
        
        float possible_draw_count_scale
        = (1.f + (float)_reduced_draw_count)
        / (1.f + (float)_draw_count);
        
        if(_draw_count && draw_count_scale < possible_draw_count_scale)
        {
            // temporarily move the object to a place it can't be seen from to test reducing draw counts
            // object expected to be culled by far view frustum plane
			msg.mesh_state = mesh_state::make();
			msg.mesh_state->visibility_sphere(sphere(glm::vec3(0, 10e10, 0), 0.0));
        }
        else
        {
            ++_reduced_draw_count;
        }
        
        ++_draw_count;
        
        return msg;
    }
    
private:
    
    int _reduced_draw_count = 0;
    int _draw_count = 0;
};

//-------------------------------------------------------------------------
//
//
binding_args_t modify_recursive(const perf_test_batching_config& config, batch_reducer<draw_batch>& draw_batch_reducer, binding_args_t msg)
{
    return draw_batch_reducer.modify(config, msg);
}

template <typename batch_reducer_t>
binding_args_t modify_recursive(const perf_test_batching_config& config, batch_reducer_t& batch_reducer, binding_args_t msg)
{
	msg = batch_reducer.modify(config, msg);
    
    using child_t = batch_reducer<typename batch_reducer_t::child_t>;
    
    auto* child = (child_t*)batch_reducer.find_child(msg);
    
    if (!child)
    {
        child = new child_t(msg);
        
        batch_reducer.insert(std::unique_ptr<batch>(child));
    }
    
    return modify_recursive(config, *child, msg);
}

//-------------------------------------------------------------------------
//
//
reduce_batching_system_state_changes::draw_pass_args_t reduce_batching_system_state_changes::modify(draw_pass_args_t msg)
{
    // only consider modifying the geometry instances that are on screen
    if(math::visibility_test(_view_frustum, msg.binding_args.mesh_state->visibility_sphere()))
    {
        if(auto scene_stage_index = msg.technique.scene_stage_index())
        {
            std::shared_ptr<batch_reducer<batches>>& scene_stage_reducer = _scene_stage_reducers[scene_stage_index];
            
            if(!scene_stage_reducer)
            {
                scene_stage_reducer = std::make_shared<batch_reducer<batches>>();
            }
            
			msg.binding_args = modify_recursive(_config, *scene_stage_reducer, msg.binding_args);
        }
    }
    
    return msg;
}

std::shared_ptr<reduce_batching_system_state_changes> reduce_batching_system_state_changes::make(const perf_test_batching_config& config, const frustum& view_frustum)
{
    return std::shared_ptr<reduce_batching_system_state_changes>(new reduce_batching_system_state_changes(config, view_frustum));
}

reduce_batching_system_state_changes::reduce_batching_system_state_changes(const perf_test_batching_config& config, const frustum& view_frustum)
: _view_frustum(view_frustum)
, _config(config)
{
}
