
#pragma once

#include <array>

namespace igpu
{	
	class geometry;
	class program;

	// pass in program and geometry,
	// get back all buffer and attribute indices inside the geometry for each program vertex_parameter.
	class attribute_finder
	{
	public:

		struct attribute_source
		{
			unsigned char buffer_index : 4;
			unsigned char attribute_index : 4;
		};

		bool find_all_attributes(
			const program&,
			const geometry&);

		const std::array< attribute_source, 16 >&  attribute_sources() const;

	private:

		std::array< attribute_source, 16 > _attribute_sources;
	};
}
