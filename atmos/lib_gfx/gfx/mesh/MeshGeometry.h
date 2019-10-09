
#pragma once

#include <framework/utility/buffer_view.h>

#include <igpu/resource/topology.h>
#include <igpu/resource/vertex_format.h>

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
        
        const buffer_view<char>& vertex_resource() const;
        
		buffer_view<char>& vertex_resource();
        
        const buffer_view<uint16_t>& index_resource() const;
        
        static std::shared_ptr<mesh_geometry> combine(const std::vector<std::shared_ptr<mesh_geometry>>&);
        
        static std::unique_ptr<mesh_geometry> make(
			const std::string&,
            igpu::topology,
            const igpu::vertex_format::msg&,
            const buffer_view_base& vertex_resource,
            const buffer_view_base& index_resource,
            const sphere&,
            const AABB&);
        
    private:
        
        mesh_geometry(
			const std::string&,
            igpu::topology,
            const igpu::vertex_format::msg&,
            const buffer_view_base& vertex_resource,
            const buffer_view_base& index_resource,
            const sphere&,
            const AABB&);
        
        
    private:
        
        // core geometry data for rendering
        std::string _name;
        igpu::topology _topology;
        igpu::vertex_format::msg _format_msg;
        allocated_safe_buffer<char> _vertex_resource;
        allocated_safe_buffer<uint16_t> _index_resource;
        
        // auxiliary data
        sphere _sphere;
        AABB _aabb;
        
        mesh_geometry (const mesh_geometry&) = delete;
        mesh_geometry& operator= (const mesh_geometry&) = delete;
    };
}
