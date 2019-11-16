
#pragma once

#include "igpu/buffer/topology.h"
#include "igpu/buffer/vertex_buffer.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class draw_target;
	class attribute_indexer;
	class program;
	class render_states;

	class graphics_pipeline
	{
	public:
		struct config
		{
			scoped_ptr< draw_target > draw_target;
			scoped_ptr< program > program;
			scoped_ptr< render_states > render_states;
			igpu::topology topology = igpu::topology::UNDEFINED;
			std::vector< vertex_buffer::config > compact_vertex_format;

			static size_t hash( const config& );

			static ptrdiff_t compare( const config&, const config& );
		};

		virtual ~graphics_pipeline() = default;

		virtual const config& cfg() const = 0;

		static config make_config(
			const attribute_indexer&,
			const scoped_ptr< draw_target >&,
			const scoped_ptr< program >&,
			const scoped_ptr< render_states >& );
	};
}
