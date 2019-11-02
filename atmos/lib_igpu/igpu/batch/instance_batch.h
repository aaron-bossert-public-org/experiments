
#pragma once

#include <igpu/utility/utility_types.h>

#include <memory>
#include <optional>
#include <vector>

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
			std::shared_ptr<program> program;
			std::shared_ptr<render_states> render_states;
			std::shared_ptr<primitives> material;
			std::shared_ptr<geometry> geometry;
			std::shared_ptr<primitives> primitives;
		};

		using item_t = batch_binding;

		virtual void element_start(const std::optional < size_t >&) = 0;

		virtual const std::optional < size_t >& element_start() const = 0;

		virtual void element_count(const std::optional < size_t >&) = 0;

		virtual const std::optional < size_t >& element_count() const = 0;

		virtual void instance_count(const std::optional < size_t >&) = 0;

		virtual const std::optional < size_t >& instance_count() const = 0;

		virtual const utility::sphere& visibility_sphere() const = 0;

		virtual void visibility_sphere(const utility::sphere& visibility_sphere) = 0;
	};
}
