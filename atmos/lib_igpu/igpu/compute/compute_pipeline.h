
#pragma once

#include "igpu/shader/constant_parameter.h"
#include "igpu/shader/constants.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class draw_target;
	class attribute_indexer;
	class compute_program;
	class render_states;

	class compute_pipeline
	{
	public:
		struct config
		{
			scoped_ptr< compute_program > program;
			std::vector< constant_parameter::config > compact_constants;

			size_t hash() const;

			ptrdiff_t compare( const config& ) const;
		};

		virtual ~compute_pipeline() = default;

		virtual const config& cfg() const = 0;

		static config make_config(
			const constants::config& constants,
			const scoped_ptr< compute_program >& );
	};
}
