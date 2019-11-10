
#pragma once

#include "igpu/utility/utility_types.h"

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
			std::shared_ptr< program > program;
			std::shared_ptr< render_states > render_states;
			std::shared_ptr< primitives > material;
			std::shared_ptr< geometry > geometry;
			std::shared_ptr< primitives > primitives;
		};

		using item_t = batch_binding;

		virtual void enabled( bool ) = 0;

		virtual void base_vertex( const std::optional< ptrdiff_t >& ) = 0;

		virtual void instance_start( const std::optional< size_t >& ) = 0;

		virtual void instance_count( const std::optional< size_t >& ) = 0;

		virtual void element_start( const std::optional< size_t >& ) = 0;

		virtual void element_count( const std::optional< size_t >& ) = 0;

		virtual void visibility_sphere(
			const std::optional< utility::sphere >& ) = 0;

		virtual bool enabled() const = 0;

		virtual const std::optional< ptrdiff_t >& base_vertex() const = 0;

		virtual const std::optional< size_t >& instance_start() const = 0;

		virtual const std::optional< size_t >& instance_count() const = 0;

		virtual const std::optional< size_t >& element_start() const = 0;

		virtual const std::optional< size_t >& element_count() const = 0;

		virtual const std::optional< utility::sphere >& visibility_sphere()
			const = 0;

		virtual ~instance_batch() = default;
	};
}
