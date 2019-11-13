
#pragma once

#include "igpu/buffer/topology.h"
#include "igpu/buffer/vertex_buffer.h"

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
			std::vector< vertex_buffer::config > verrtex_format;
			igpu::topology topology;
		};

		virtual ~graphics_pipeline() = default;

		virtual const config& cfg() const = 0;

		static config make_config( const attribute_indexer& );

		static size_t hash( const config& );

		static bool equal( const config&, const config& );
	};
}
