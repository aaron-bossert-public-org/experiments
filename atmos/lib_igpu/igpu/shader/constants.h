
#pragma once

#include "igpu/shader/constant.h"

#include <vector>

namespace igpu
{
	class constants
	{
	public:
		struct config
		{
			std::vector< constant > constants;

			size_t find( const std::string_view& ) const;

			size_t hash() const;

			ptrdiff_t compare( const config& ) const;
		};

		virtual const config& cfg() const = 0;

		virtual ~constants() = default;
	};
}
