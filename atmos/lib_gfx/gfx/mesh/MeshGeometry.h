
#pragma once

#include <framework/utility/buffer_view.h>

#include <igpu/buffer/topology.h>
#include <igpu/buffer/vertex_format.h>

#include <glm/fwd.hpp>

namespace gfx
{
    class mesh_geometry final
    {
    public:
        
        mesh_geometry();
        
        const std::string& name() const;
        
        igpu::topology topology() const;
        
        const sphere& bounding_sphere() const;
        
        const AABB& bounding_aabb() const;
        
        int32_t vertex_count() const;
        
        int32_t vertex_stride() const;
        
        int32_t index_count() const;
        
        const igpu::vertex_format::msg& format_msg() const;
        
        const buffer_view<char>& vertex_buffer() const;
        
		buffer_view<char>& vertex_buffer();
        
        const buffer_view<uint16_t>& index_buffer() const;
        
        static std::shared_ptr<mesh_geometry> combine(const std::vector<std::shared_ptr<mesh_geometry>>&);
        
        static std::unique_ptr<mesh_geometry> make(
			const std::string&,
            igpu::topology,
            const igpu::vertex_format::msg&,
            const buffer_view_base& vertex_buffer,
            const buffer_view_base& index_buffer,
            const sphere&,
            const AABB&);
        
    private:
        
        mesh_geometry(
			const std::string&,
            igpu::topology,
            const igpu::vertex_format::msg&,
            const buffer_view_base& vertex_buffer,
            const buffer_view_base& index_buffer,
            const sphere&,
            const AABB&);
        
        
    private:
        
        // core geometry data for rendering
        std::string _name;
        igpu::topology _topology;
        igpu::vertex_format::msg _format_msg;
        allocated_safe_buffer<char> _vertex_buffer;
        allocated_safe_buffer<uint16_t> _index_buffer;
        
        // auxiliary data
        sphere _sphere;
        AABB _aabb;
        
        mesh_geometry (const mesh_geometry&) = delete;
        mesh_geometry& operator= (const mesh_geometry&) = delete;
    };
}
