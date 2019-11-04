
#pragma once

#include "igpu/material/primitive.h"
#include "gfx/material/parameter_binding.h"

#include <unordered_map>

namespace igpu
{
	using value_hash_t = size_t;
    class packed_texture;
    class parameter;
    class program;
}

namespace gfx
{
	class parameter_block final
    {
    public:
        
        using parameter_handle = size_t;
        using handles_t = std::unordered_map<parameter_id, parameter_handle>;
        using primitives_t = std::vector<igpu::primitive>;
        
    public:
        
        parameter_block(bool slow_hash);
        
        parameter_block(const parameter_block& other, bool slow_hash);
        
        parameter_block(std::vector<std::pair<parameter_id, igpu::primitive>>, bool slow_hash);
        
        parameter_block& operator=(const parameter_block& other);
        
        parameter_handle lookup(const parameter_id& name) const;
        
        void set(const parameter_block& block);
        
        void set(const std::vector<std::pair<parameter_id, igpu::primitive>>& config);
        
        parameter_handle set(const parameter_id&, const igpu::primitive&);
        
        void set(const parameter_handle&, const igpu::primitive&);
        
        const igpu::primitive& get(const parameter_id& name) const;
        
        const igpu::primitive& get(const parameter_handle& handle) const;
        
        const handles_t& handles() const;
        
        const primitives_t& primitives() const;
        
        const igpu::value_hash_t& hash() const;
        
        parameter_block_binding_id binding_id() const;
        
        bool slow_hash() const;
        
    private:
        
        bool _slow_hash;
		igpu::value_hash_t _hash;
        parameter_block_binding_id _binding_id = -1;
        handles_t _handles;
        primitives_t _primitives;
    };
}
