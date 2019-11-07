
#pragma once

#include <array>

namespace igpu
{
	class geometry;
	class vertex_parameters;

	// pass in vertex_parameters and geometry,
	// get back all buffer and attribute indices inside the geometry for each
	// vertex_parameter.
	class attribute_indexer
	{
	public:
		static constexpr size_t MAX_PARAMETERS = 16;

		struct attribute_source
		{
			unsigned char buffer : 4;
			unsigned char attribute : 4;
		};

		bool reset( const vertex_parameters&, const geometry& );

		const std::array< attribute_source, attribute_indexer::MAX_PARAMETERS >&
			indices() const;

	private:
		std::array< attribute_source, attribute_indexer::MAX_PARAMETERS >
			_indices;
	};
}
