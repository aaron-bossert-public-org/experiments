
#pragma once

#include <igpu/utility/utility_types.h>

#include <memory>
#include <vector>

namespace igpu
{
	class batch_binding;
	class geometry;
	class material;
	class primitive_kv;
	class program;
	class render_states;

	class instance_batch
	{
	public:

		using item_t = batch_binding;

		struct config
		{
			std::shared_ptr<program> program;
			std::shared_ptr<render_states> render_states;
			std::shared_ptr<material> material;
			std::shared_ptr<geometry> geometry;
			std::vector<primitive_kv> primitives;
		};

		virtual const config& cfg() const = 0;

		virtual void count(size_t) = 0;

		virtual size_t count() const = 0;

		virtual const utility::sphere& visibility_sphere() const = 0;

		virtual void visibility_sphere(const utility::sphere& visibility_sphere) = 0;
	};
}
