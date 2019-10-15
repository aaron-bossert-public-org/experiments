
#pragma once

#include <igpu/buffer/vertex_parameter.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace igpu
{
    class vertex_constraints
    {
    public:

		static constexpr size_t NOT_FOUND = (size_t)-1;

		struct config
		{
			std::vector<vertex_parameter> vertex_parameters;
		};

		vertex_constraints(const config&);

		const config& cfg() const;

		const vertex_parameter& find_parameter(const std::string&, size_t fallback = NOT_FOUND) const;

		size_t find_location(const std::string&, size_t fallback = NOT_FOUND) const;

    protected:
		
		config _cfg;
		std::unordered_map<std::string, size_t> _locations;
    };
}
