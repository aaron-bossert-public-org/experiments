
#pragma once

#include "igpu/shader/vertex_parameters.h"

#include <array>

namespace igpu
{
	class geometry;
	class vertex_parameters;

	// pass in vertex_parameters and geometry,
	// get back all buffer and attribute indices inside the geometry for each
	// vertex_parameter.
	class attribute_indexer final
	{
	public:
		struct attribute_source
		{
			enum
			{
				BITS = 4,
			};

			unsigned char compact_buffer_index : BITS;
			unsigned char attribute_index : BITS;
		};

		using buffer_indices_t =
			std::array< uint8_t, vertex_parameters::MAX_COUNT >;

		using indices_t =
			std::array< attribute_source, vertex_parameters::MAX_COUNT >;

		bool reset( const vertex_parameters&, const geometry& );

		const igpu::vertex_parameters* vertex_parameters() const;

		const igpu::geometry* geometry() const;

		const size_t buffer_count() const;

		const buffer_indices_t& buffer_indices() const;

		const indices_t& indices() const;

	private:
		const igpu::vertex_parameters* _vertex_parameters = nullptr;

		const igpu::geometry* _geometry = nullptr;

		size_t _buffer_count;

		buffer_indices_t _buffer_indices;

		indices_t _indices;
	};
}
