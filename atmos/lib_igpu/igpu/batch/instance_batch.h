
#pragma once

#include "igpu/batch/draw_parameters.h"
#include "igpu/utility/utility_types.h"

#include <memory>
#include <variant>

namespace igpu
{
	class batch_binding;
	class program;
	class render_states;
	class geometry;
	class primitives;

	class instance_batch
	{
	public:
		struct config
		{
			std::shared_ptr< program > program;
			std::shared_ptr< render_states > states;
			std::shared_ptr< geometry > geometry;
			std::shared_ptr< primitives > material;
			std::shared_ptr< primitives > instance;
		};

		struct no_draw_params
		{};

		using variant_t = std::variant<
			no_draw_params,
			draw_parameters,
			draw_indirect_parameters >;

		using item_t = batch_binding;

		virtual void enabled( bool ) = 0;

		virtual bool enabled() const = 0;

		virtual void draw_params( const variant_t& ) = 0;

		virtual const variant_t& draw_params() const = 0;

		virtual ~instance_batch() = default;
	};
}
