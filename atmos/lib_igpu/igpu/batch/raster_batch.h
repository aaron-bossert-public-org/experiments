
#pragma once

#include "igpu/batch/raster_binding.h"
#include "igpu/shader/constants.h"
#include "igpu/shader/primitives.h"
#include "igpu/utility/utility_types.h"

#include "framework/utility/scoped_ptr.h"

#include <functional>
#include <memory>

namespace igpu
{
	class draw_target;

	class raster_batch
	{
	public:
		struct config
		{
			scoped_ptr< draw_target > draw_target;
			std::shared_ptr< igpu::primitives > primitives;
			constants::config constants;
		};

		struct raster_state
		{
			utility::frustum frustum;
		};

		virtual const config& cfg() const = 0;

		virtual std::unique_ptr< raster_binding > make_binding(
			const instance_batch::config& ) = 0;

		virtual void raster(
			const scoped_ptr< draw_target >&,
			const raster_state& ) = 0;

		void visit(
			const std::function< void( const instance_batch& ) >& visit );

		virtual ~raster_batch() = default;

	protected:
		raster_batch() = default;
		raster_batch( const raster_batch& ) = delete;
		raster_batch& operator=( const raster_batch& ) = delete;
	};
}
