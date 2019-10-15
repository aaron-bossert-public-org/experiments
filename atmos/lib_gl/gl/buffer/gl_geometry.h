
#pragma once

#include <gl/buffer/gl_index_buffer.h>
#include <gl/buffer/gl_vertex_format.h>
#include <igpu/buffer/geometry.h>

namespace igpu
{
    class gl_geometry : public geometry
    {
    public:

		const config& cfg() const override;
		
		size_t element_start() const override;

		size_t element_count() const override;

		void element_start(size_t) override;

		void element_count(size_t) override;

		unsigned gl_topology() const;

        unsigned vao() const;

		const gl_vertex_format& vertex_format();

        static std::unique_ptr<gl_geometry> make(
			const config&);
        
        ~gl_geometry() override;
        
    protected:

        gl_geometry(
			const config&,
			unsigned gl_toplogy,
			unsigned vao);
        
    private:
        
		const config _cfg;
		size_t _element_start;
		size_t _element_count;
		unsigned _gl_topology = 0;
		unsigned _vao = 0;
    };
}
