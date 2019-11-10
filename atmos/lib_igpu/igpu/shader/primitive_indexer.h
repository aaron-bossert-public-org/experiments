
#pragma once

#include "igpu/shader/parameters.h"

#include <array>

namespace igpu
{
	class parameters;
	class primitives;

	// pass in parameters and geometry,get back all buffer and primitive indices
	// inside the geometry for each parameters.
	class primitive_indexer final
	{
	public:
		bool reset( const parameters&, const primitives& );

		const std::array< uint8_t, parameters::MAX_COUNT >& indices() const;

	private:
		std::array< uint8_t, parameters::MAX_COUNT > _indices;
	};
}
