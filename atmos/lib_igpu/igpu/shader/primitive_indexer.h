
#pragma once

#include <array>

namespace igpu
{
	class parameters;
	class primitives;

	// pass in parameters and geometry,get back all buffer and primitive indices
	// inside the geometry for each parameters.
	class primitive_indexer
	{
	public:
		static constexpr size_t MAX_PARAMETERS = 64;

		bool reset( const parameters&, const primitives& );

		const std::array< uint8_t, MAX_PARAMETERS >& indices() const;

	private:
		std::array< uint8_t, MAX_PARAMETERS > _indices;
	};
}
