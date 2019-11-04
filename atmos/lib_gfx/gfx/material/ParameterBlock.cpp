
#include "gfx/material/parameter_block.h"

#include "framework/logging/log.h"
#include "igpu/material/program.h"
#include "igpu/material/parameter.h"
#include "igpu/texture/render_texture_2d.h"
#include "igpu/texture/texture_2d.h"

using namespace gfx;

namespace
{
    int next_unique_id()
    {
        static int s_unique_id = 0;
        
        return ++s_unique_id;
    }
    
    int next_binding_id()
    {
        static int s_next_binding_id = 0;
        
        return ++s_next_binding_id;
    }
}

parameter_block::parameter_block(bool slow_hash)
: _hash(igpu::value_hash_t(next_unique_id()))
, _binding_id(next_binding_id())
, _slow_hash(slow_hash)
{
}

parameter_block::parameter_block(const parameter_block& other, bool slow_hash)
: _hash(igpu::value_hash_t(next_unique_id()))
, _binding_id(next_binding_id())
, _slow_hash(slow_hash)
{
    *this = other;
}

parameter_block::parameter_block(std::vector<std::pair<parameter_id, igpu::primitive>> config, bool slow_hash)
: parameter_block(slow_hash)
{
    set(config);
}

parameter_block& parameter_block::operator=(const parameter_block& other)
{
    set(other);
    
    return *this;
}

parameter_block::parameter_handle parameter_block::lookup(const parameter_id& name) const
{
    auto find = _handles.find(name);
    
    if(find != _handles.end())
    {
        return find->second;
    }
    
    return (size_t)-1;
}

void parameter_block::set(const parameter_block& block)
{
    const auto& primitives = block.primitives();
    
    for (const auto& handle : block.handles())
    {
        const auto& name = handle.first;
        size_t index = handle.second;
        
        set(name, primitives[index]);
    }
}

void parameter_block::set(const std::vector<std::pair<parameter_id, igpu::primitive>>& config)
{
    for (auto& pair : config)
    {
        set(pair.first, pair.second);
    }
}

parameter_block::parameter_handle parameter_block::set(const parameter_id& name, const igpu::primitive& primitive)
{
    parameter_handle handle = lookup(name);
    
    if (handle != -1)
    {
        set(handle, primitive);
    }
    else// no existing parameter with that name, add a new one
    {
        _binding_id = next_binding_id();
        
        // index of the parameter in the array of parameters
        handle = _primitives.size();
        _handles.emplace(name, handle);
        
        // add parameter to _parameter_bindings, notify block that parameters need rebinding
        _primitives.emplace_back(primitive);
		
        auto& p = _primitives.back();
        
        p.hash( _slow_hash ? igpu::primitive::make_hash(p) : _hash);
    }
    
    return handle;
}

void parameter_block::set(const parameter_handle& handle, const igpu::primitive& primitive_)
{
    if (handle != -1)
    {
        ASSERT_CONTEXT(handle >= 0 && handle < _primitives.size());
        auto& primitive = _primitives[handle];
        primitive = primitive_;
        
        primitive.hash( _slow_hash ? igpu::primitive::make_hash(primitive) : _hash);
    }
}

const igpu::primitive& parameter_block::get(const parameter_id& name) const
{
    return get(lookup(name));
}

const igpu::primitive& parameter_block::get(const parameter_handle& handle) const
{
    if(-1 != handle)
    {
        return _primitives[handle];
    }
    
    static igpu::primitive s_fallback;
    return s_fallback;
}

const parameter_block::handles_t& parameter_block::handles() const
{
    return _handles;
}

const parameter_block::primitives_t& parameter_block::primitives() const
{
    return _primitives;
}

const igpu::value_hash_t& parameter_block::hash() const
{
    return _hash;
}

parameter_block_binding_id parameter_block::binding_id() const
{
    return _binding_id;
}

bool parameter_block::slow_hash() const
{
    return _slow_hash;
}
