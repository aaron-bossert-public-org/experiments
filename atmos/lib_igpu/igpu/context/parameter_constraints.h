
#pragma once

#include "igpu/shader/primitive.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace igpu
{
    class parameter_constraints
    {
    public:
		struct primitive_kv {};
		struct config
		{
			std::vector<primitive_kv> parameters;
		};

		parameter_constraints(const config&);

		const config& cfg() const;

		size_t index_of(const std::string_view&) const;

    protected:
		
		config _cfg;
		std::unordered_map<std::string_view, size_t> _lookup;
    };
}
