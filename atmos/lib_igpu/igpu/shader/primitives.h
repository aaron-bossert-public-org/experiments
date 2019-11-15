
#pragma once

#include "igpu/shader/primitive.h"

#include <vector>

namespace igpu
{
	class primitives
	{
	public:
		struct config
		{
			std::vector< primitive::config > primitives;

			static size_t hash( const config& );

			static ptrdiff_t compare( const config&, const config& );
		};

		virtual const config& cfg() const = 0;

		virtual size_t primitive_count() const = 0;

		virtual const igpu::primitive& primitive( size_t ) const = 0;

		bool find_expected( const std::string&, size_t* p_expected ) const;

		virtual ~primitives() = default;
	};
}
