
#pragma once

#include <igpu/buffer/vertex_parameter.h>
#include <string>
#include <vector>

namespace igpu
{   
	struct vertex_attribute
	{
		vertex_parameter parameter;
		uint32_t offset = 0;
	};
	
	class vertex_format
	{
	public:

		struct config
		{
			uint32_t stride = 0;
			std::vector<vertex_attribute> attributes;
		};

		const config& cfg() const;

		~vertex_format() = default;
		vertex_format(const config&);

	private:

		config _cfg;
	};
}
