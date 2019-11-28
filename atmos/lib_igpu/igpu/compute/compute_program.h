
#pragma once

#include "igpu/shader/constants.h"

#include <memory>

namespace igpu
{
	class compute_shader;
	class constant_parameters;
	class parameters;

	class compute_program
	{
	public:
		struct config
		{
			std::string name;
			std::shared_ptr< compute_shader > compute;
			constants::config constants;

			size_t hash() const;

			ptrdiff_t compare( const config& ) const;
		};

		virtual const config& cfg() const = 0;

		virtual const igpu::constant_parameters& constant_parameters()
			const = 0;

		virtual const parameters& batch_parameters() const = 0;

		virtual const parameters& material_parameters() const = 0;

		virtual const parameters& instance_parameters() const = 0;

		virtual ~compute_program() = default;
	};
}
