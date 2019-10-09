
#pragma once

namespace gfx
{
    template<typename vertex, typename index>
    class primitive_container
    {
    public:
        using vertex_t = vertex;
        using index_t = index;
        
        primitive_container() {}
        
        primitive_container (primitive_container &&) = default;
        
        primitive_container(const primitive_container& other) = default;
        
        primitive_container& operator= (const primitive_container&) = default;
        
        template<typename generator>
        primitive_container(const generator& generator);
        
        template<typename generator>
        primitive_container& operator= (const generator& generator);
        
        template<typename prim_container>
        primitive_container& combine (const prim_container& primitive_container);
        
        primitive_container& combine (const primitive_container& primitive_container);
        
        void release();
        
        const std::vector<index>& indices() const;
        
        const std::vector<vertex>& vertices() const;
        
        std::vector<index>& indices();
        
        std::vector<vertex>& vertices();

    private:
        
        std::vector<index> _indices;
        std::vector<vertex> _vertices;
    };
    
    template<typename vertex, typename index>
    template<typename generator>
    primitive_container<vertex, index>::primitive_container(const generator& generator)
    {
        *this = generator;
    }
    
    template<typename vertex, typename index>
    void primitive_container<vertex, index>::release()
    {
        // trigger release of reserved memory normally held onto for rapid
        std::swap(_indices, std::vector<index>());
        std::swap(_vertices, std::vector<vertex>());
    }
    
    template<typename vertex, typename index>
    template<typename generator>
    primitive_container<vertex, index>& primitive_container<vertex, index>::operator= (const generator& generator)
    {
        generator.get_indices(&_indices);
        generator.get_vertices(&_vertices);
        
        return *this;
    }
    
    template<typename vertex, typename index>
    template<typename prim_container>
    primitive_container<vertex, index>& primitive_container<vertex, index>::combine(const prim_container& other)
    {
        unsigned vertex_start = _vertices.size();
        unsigned index_start = _indices.size();
        
        _vertices.resize(_vertices.size() + other.vertices().size());
        _indices.resize(_indices.size() + other.indices().size());
        
        std::transform(other.vertices().begin(), other.vertices().end(), _vertices.begin() + vertex_start, [](const typename prim_container::vertex_t& vertex){
            
            vertex cast_vertex;
            cast_vertex.cast_from(vertex);
            
            return cast_vertex;
        });
        
        std::transform(other.indices().begin(), other.indices().end(), _indices.begin() + index_start, [vertex_start](const typename prim_container::index_t index){
            
            return index + vertex_start;
        });
        
        return *this;
    }
    
    template<typename vertex, typename index>
    primitive_container<vertex, index>& primitive_container<vertex, index>::combine(const primitive_container& other)
    {
        unsigned vertex_start = _vertices.size();
        unsigned index_start = _indices.size();
        
        _vertices.resize(_vertices.size() + other.vertices().size());
        _indices.resize(_indices.size() + other.indices().size());
        
        std::copy(other.vertices().begin(), other.vertices().end(), _vertices.begin() + vertex_start);
        
        std::transform(other.indices().begin(), other.indices().end(), _indices.begin() + index_start, [vertex_start](const index_t index){
            
            return index + vertex_start;
        });
        
        return *this;
    }
    
    template<typename vertex, typename index>
    const std::vector<index>& primitive_container<vertex, index>::indices() const
    {
        return _indices;
    }
    
    template<typename vertex, typename index>
    const std::vector<vertex>& primitive_container<vertex, index>::vertices() const
    {
        return _vertices;
    }
    
    template<typename vertex, typename index>
    std::vector<index>& primitive_container<vertex, index>::indices()
    {
        return _indices;
    }
    
    template<typename vertex, typename index>
    std::vector<vertex>& primitive_container<vertex, index>::vertices()
    {
        return _vertices;
    }
}
