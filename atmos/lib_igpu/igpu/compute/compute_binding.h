
#pragma once

#include "igpu/compute/compute_parameters.h"
#include "igpu/shader/constants.h"

#include <memory>

namespace igpu
{
	class compute_program;
	class primitives;

	class compute_binding
	{
	public:
		struct config
		{
			std::shared_ptr< compute_program > program;
			std::shared_ptr< primitives > batch;
			std::shared_ptr< primitives > material;
			std::shared_ptr< primitives > instance;
			constants::config constants;
		};

		virtual const config& cfg() const = 0;

		virtual void dispatch() = 0;

		virtual void compute_params( const compute_parameters& ) = 0;

		virtual const compute_parameters& compute_params() const = 0;

		virtual ~compute_binding() = default;
	};
}
