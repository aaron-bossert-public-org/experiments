
#pragma once

#include "igpu/buffer/topology.h"
#include "igpu/buffer/vertex_buffer.h"

#include <memory>

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
			std::shared_ptr< draw_target > draw_target;
			std::shared_ptr< program > program;
			std::shared_ptr< render_states > render_states;
			igpu::topology topology = igpu::topology::UNDEFINED;
			std::vector< vertex_buffer::config > compact_vertex_format;

			static size_t hash( const config& );

			static ptrdiff_t compare( const config&, const config& );
		};

		virtual ~graphics_pipeline() = default;

		virtual const config& cfg() const = 0;

		virtual void on_back_buffer_resized( const glm::ivec2& ) = 0;

		static config make_config(
			const attribute_indexer&,
			const std::shared_ptr< draw_target >&,
			const std::shared_ptr< program >&,
			const std::shared_ptr< render_states >& );
	};
}
