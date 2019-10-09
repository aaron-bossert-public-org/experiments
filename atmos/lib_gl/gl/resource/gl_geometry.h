
#pragma once

#include <igpu/resource/geometry.h>
#include <gl/resource/gl_index_resource.h>
#include <gl/resource/gl_vertex_format.h>

namespace igpu
{
    class gl_geometry : public geometry
    {
    public:

		unsigned gl_topology() const;

        unsigned vao() const;

		const gl_vertex_format& vertex_format();

        static std::unique_ptr<gl_geometry> make(
			std::string name,
			igpu::topology,
			size_t element_start,
			size_t element_count,
			std::shared_ptr<igpu::index_resource>,
			std::vector<std::shared_ptr<vertex_resource>>);
        
        ~gl_geometry() override;
        
    protected:

        gl_geometry(
			std::string name,
			igpu::topology,
			size_t element_start,
			size_t element_count,
			unsigned gl_toplogy,
			unsigned vao,
			std::shared_ptr<igpu::index_resource>,
			std::vector<std::shared_ptr<vertex_resource>>);
        
    private:
        
		unsigned _gl_topology = 0;
		unsigned _vao = 0;
    };
}
