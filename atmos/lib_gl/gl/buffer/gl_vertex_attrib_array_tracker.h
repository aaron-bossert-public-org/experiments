
#pragma once

#include <memory>
#include <vector>

namespace igpu
{
    class gl_program;
    class gl_geometry;
    
    class gl_vertex_attrib_array_tracker
    {
    public:
        
        gl_vertex_attrib_array_tracker();
        
        ~gl_vertex_attrib_array_tracker();
        
        int max_vertex_attrib_index() const;
        
        void state_caching_enabled(bool);
        
        bool state_caching_enabled() const;
        
        void set_active_vertex_buffers(const gl_geometry&);
        
    protected:

		enum class array_state;

        bool _state_caching_enabled = false;
        int _max_vertex_attrib_index;
        std::vector<unsigned> _enabled_arrays;
        std::vector<unsigned> _next_enabled_arrays;
        std::vector<array_state> _array_state;
    };
}
