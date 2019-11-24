
#pragma once

#include "igpu/batch/draw_parameters.h"
#include "igpu/buffer/topology.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace igpu
{
	class index_buffer;
	class vertex_buffer;

	class geometry
	{
	public:
		struct config
		{
			std::string name;
			igpu::topology topology;
			std::shared_ptr< igpu::index_buffer > index_buffer;
			std::vector< std::shared_ptr< vertex_buffer > > vertex_buffers;
			size_t ibuff_byte_offset = 0;
			std::vector< size_t > vbuff_byte_offsets;

			static size_t hash( const config& );

			static ptrdiff_t compare( const config&, const config& );
		};

		virtual const config& cfg() const = 0;

		virtual igpu::draw_parameters draw_parameters() const = 0;

		virtual igpu::index_buffer& index_buffer() = 0;

		virtual const igpu::index_buffer& index_buffer() const = 0;

		virtual size_t vertex_buffer_count() const = 0;

		virtual igpu::vertex_buffer& vertex_buffer( size_t ) = 0;

		virtual const igpu::vertex_buffer& vertex_buffer( size_t ) const = 0;

		virtual ~geometry() = default;

		bool find_expected_vertex_param(
			const std::string& name,
			size_t* p_expected_buff,
			size_t* p_expected_attr ) const;
	};
}
