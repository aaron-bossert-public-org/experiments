
#include <gfx/mesh/mesh_geometry.h>

#include <gfx/mesh/hull.h>
#include <framework/logging/log.h>

using namespace gfx;

const std::string& mesh_geometry::name() const
{
    return _name;
}

igpu::topology mesh_geometry::topology() const
{
    return _topology;
}

const sphere& mesh_geometry::bounding_sphere() const
{
    return _sphere;
}

const AABB& mesh_geometry::bounding_AABB() const
{
    return _aabb;
}

int32_t mesh_geometry::vertex_count() const
{
    return _vertex_resource.size();
}

int32_t mesh_geometry::vertex_stride() const
{
    return _vertex_resource.stride();
}

int32_t mesh_geometry::index_count() const
{
    return _index_resource.size();
}

const igpu::vertex_format::msg& mesh_geometry::format_msg() const
{
    return _format_msg;
}

const buffer_view<char>& mesh_geometry::vertex_resource() const
{
    return _vertex_resource;
}

buffer_view<char>& mesh_geometry::vertex_resource()
{
    return _vertex_resource;
}

const buffer_view<uint16_t>& mesh_geometry::index_resource() const
{
    return _index_resource;
}

std::shared_ptr<mesh_geometry> mesh_geometry::combine(const std::vector<std::shared_ptr<mesh_geometry>>& mesh_geometries)
{
    const size_t size = mesh_geometries.size();
    
    // handle degenerate cases up front.
    if(!size) return nullptr;
    if(1 == size) return mesh_geometries[0];
    
    
    std::string name;
    igpu::topology topology;
    const igpu::vertex_format::msg* format_msg = nullptr;
    sphere sphere;
    AABB aabb;
    int vert_count = 0;
    int vert_stride = 0;
    int index_count = 0;
    int index_stride = 0;
    
    bool first = true;
    for(const auto& mesh_geometry : mesh_geometries)
    {
        if(first)
        {
            first = false;
            
            name = mesh_geometry->name();
			topology = mesh_geometry->topology();
			format_msg = &mesh_geometry->format_msg();
            sphere = mesh_geometry->bounding_sphere();
            aabb = mesh_geometry->bounding_AABB();
            vert_stride = mesh_geometry->vertex_resource().stride();
            index_stride = mesh_geometry->index_resource().stride();
        }
        else
        {
            name = name + ";" + mesh_geometry->name();
            
            ASSERT_CONTEXT(topology == mesh_geometry->topology());
            
#if ATMOS_DEBUG
            for( size_t attrib = 0; attrib < igpu::attrib_count; ++attrib )
            {
                const auto* first_element = format_msg->element(attrib);
                const auto* current_element = mesh_geometry->format_msg().element(attrib);
                
                ASSERT_CONTEXT(!first_element == !current_element);
                
                if(first_element && current_element)
                {
                    ASSERT_CONTEXT(first_element->offset() == current_element->offset());
                    ASSERT_CONTEXT(first_element->location() == current_element->location());
                    ASSERT_CONTEXT(first_element->type() == current_element->type());
                }
            }
#endif
            
            ASSERT_CONTEXT(vert_stride == mesh_geometry->vertex_resource().stride());
            ASSERT_CONTEXT(index_stride == mesh_geometry->index_resource().stride());
            
            sphere = math::combine(sphere, mesh_geometry->bounding_sphere());
            aabb = combine_aabb(aabb, mesh_geometry->bounding_aabb());
        }
        
        vert_count += mesh_geometry->vertex_count();
        index_count += mesh_geometry->index_count();
    }
    
    allocated_safe_buffer<uint16_t> merged_indices(index_count, index_stride);
    allocated_safe_buffer<uint8_t> merged_vertices(vert_count, vert_stride);
    
    auto output_indices = merged_indices.begin();
    auto output_vertices = merged_vertices.begin();
    
    for(const auto& mesh_geometry : mesh_geometries)
    {
        // merge input index resource: new indices will be corrected to match indices of vertices in merged vertex resource
        const auto& input_indices = mesh_geometry->index_resource();
        
        const uint16_t written_verts = output_vertices - merged_vertices.begin();
        std::transform(input_indices.begin(), input_indices.end(), output_indices, [written_verts]( const uint16_t& index ) {
            
            return index + written_verts;
        });

        output_indices += input_indices.size();
        
        
        // merge input vertex resource
        const auto& input_vertices = mesh_geometry->vertex_resource();
        memcpy ( &*output_vertices, input_vertices.data(), merged_vertices.stride() * input_vertices.size() );
        
        output_vertices += input_vertices.size();
    }
    
    ASSERT_CONTEXT(output_indices == merged_indices.end());
    ASSERT_CONTEXT(output_vertices == merged_vertices.end());
    
    return make(
        name,
        topology,
        *format_msg,
        merged_vertices,
        merged_indices,
        sphere,
        aabb);
}

std::unique_ptr<mesh_geometry> mesh_geometry::make(
	const std::string&            name,
    igpu::topology                 topology,
    const igpu::vertex_format::msg& format_msg,
    const buffer_view_base&       vertex_resource,
    const buffer_view_base&       index_resource,
    const sphere&                 sphere,
    const AABB&                   aabb)
{
    if(name.empty())
    {
        LOG_CONTEXT( CRITICAL, "name is empty");
    }
    else if (!format_msg.elements().size())
    {
        LOG_CONTEXT( CRITICAL, "vertex format is empty: %s", name.c_str() );
    }
    else if (!vertex_resource.valid())
    {
        LOG_CONTEXT( CRITICAL, "vertex resource is not valid: %s", name.c_str() );
    }
    else if (!index_resource.valid())
    {
        LOG_CONTEXT( CRITICAL, "index resource is not valid: %s", name.c_str() );
    }
    else
    {
        return std::unique_ptr<mesh_geometry>(new mesh_geometry(
			name,
			topology,
			format_msg,
			vertex_resource,
			index_resource,
			sphere,
			aabb));
    }
    
    return nullptr;
}

mesh_geometry::mesh_geometry(
	const std::string&            name,
	igpu::topology                 topology,
	const igpu::vertex_format::msg& format_msg,
	const buffer_view_base&       vertex_resource,
	const buffer_view_base&       index_resource,
	const sphere&                 sphere,
	const AABB&                   aabb)
: _name(name)
, _topology(topology)
, _format_msg(format_msg)
, _sphere(sphere)
, _aabb(aabb)
, _vertex_resource(vertex_resource.size(), vertex_resource.stride() )
, _index_resource(index_resource.size(), index_resource.stride() )
{
    ASSERT_CONTEXT(vertex_resource.size() < std::numeric_limits<uint16_t>::max());
    
    int32_t vertex_stride = vertex_resource.stride();
    int32_t vertex_count = vertex_resource.size();
    
    ::memcpy(_vertex_resource.data(), vertex_resource.data(), vertex_count * vertex_stride);
    
    int32_t index_count = index_resource.size();
    int32_t index_stride = index_resource.stride();
    
    ::memcpy(_index_resource.data(), index_resource.data(), index_count * index_stride);
}



