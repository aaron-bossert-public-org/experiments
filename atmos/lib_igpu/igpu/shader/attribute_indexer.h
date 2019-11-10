
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
			unsigned char buffer : 4;
			unsigned char attribute : 4;
		};

		bool reset( const vertex_parameters&, const geometry& );

		const std::array< attribute_source, vertex_parameters::MAX_COUNT >&
			indices() const;

	private:
		std::array< attribute_source, vertex_parameters::MAX_COUNT > _indices;
	};
}
