
#pragma once

#include "igpu/buffer/topology.h"
#include "igpu/buffer/vertex_buffer.h"

#include <memory>

namespace igpu
{
	class attribute_indexer;
	class program;
	class render_states;

	class graphics_pipeline
	{
	public:
		struct config
		{
			std::shared_ptr< program > program;
			std::shared_ptr< render_states > render_states;
			igpu::topology topology;
			std::vector< vertex_buffer::config > compact_vertex_format;
		};

		virtual ~graphics_pipeline() = default;

		virtual const config& cfg() const = 0;

		static config make_config(
			const attribute_indexer&,
			const std::shared_ptr< program >&,
			const std::shared_ptr< render_states >& );

		static ptrdiff_t compare( const config&, const config& );
	};
}
