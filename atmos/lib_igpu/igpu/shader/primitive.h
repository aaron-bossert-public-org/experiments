
#pragma once

#include "igpu/shader/parameter.h"

#include <memory>
#include <string>
#include <variant>

namespace igpu
{
	class compute_buffer;
	class depth_texture2d;
	class index_buffer;
	class render_texture2d;
	class texture2d;
	class vertex_buffer;

	class primitive
	{
	public:
		using variant_t = std::variant<
			std::shared_ptr< compute_buffer >,
			std::shared_ptr< vertex_buffer >,
			std::shared_ptr< index_buffer >,
			std::shared_ptr< texture2d >,
			std::shared_ptr< depth_texture2d >,
			std::shared_ptr< render_texture2d > >;

		struct config
		{
			std::string name;
			variant_t value;

			size_t hash() const;

			ptrdiff_t compare( const config& ) const;
		};

		virtual const config& cfg() const = 0;

		parameter::type type() const;

		virtual ~primitive() = default;
	};
}
