
#pragma once

#include "igpu/shader/primitive.h"

namespace igpu
{
	class primitives
	{
	public:
		struct config
		{
			std::vector< primitive::config > primitives;
		};

		virtual size_t primitive_count() const = 0;

		virtual const igpu::primitive& primitive( size_t ) const = 0;

		bool find_expected( const std::string&, size_t* p_expected ) const;

		virtual ~primitives() = default;
	};
}
