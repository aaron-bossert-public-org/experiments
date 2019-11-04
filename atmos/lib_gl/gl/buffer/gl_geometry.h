
#pragma once

#include "gl/buffer/gl_index_buffer.h"
#include "gl/buffer/gl_vertex_buffer.h"

#include "igpu/buffer/geometry.h"

#include <optional>

namespace igpu
{
	class gl_geometry : public geometry
	{
	public:
		const config& cfg() const override;

		igpu::index_buffer& index_buffer() override;

		igpu::vertex_buffer& vertex_buffer( size_t ) override;

		size_t element_start() const override;

		size_t element_count() const override;

		void element_start( size_t ) override;

		void element_count( size_t ) override;

		unsigned gl_topology() const;

		unsigned vao() const;

		static std::unique_ptr< gl_geometry > make( const config& );

		~gl_geometry() override;

	protected:
		gl_geometry(
			const config&,
			gl_index_buffer* index_buffer,
			std::vector< gl_vertex_buffer* > vertex_buffers,
			unsigned gl_toplogy,
			unsigned vao );

	private:
		const config _cfg;
		gl_index_buffer* const _index_buffer;
		const std::vector< gl_vertex_buffer* > _vertex_buffers;
		std::optional< size_t > _element_start;
		std::optional< size_t > _element_count;
		unsigned _gl_topology = 0;
		unsigned _vao = 0;
	};
}
