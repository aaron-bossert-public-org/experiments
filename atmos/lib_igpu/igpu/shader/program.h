
#pragma once

#include "igpu/shader/constants.h"

#include <memory>
#include <unordered_map>

namespace igpu
{
	class fragment_shader;
	class constant_parameters;
	class parameters;
	class vertex_parameters;
	class vertex_shader;

	class program
	{
	public:
		struct config
		{
			std::string name;
			std::shared_ptr< vertex_shader > vertex;
			std::shared_ptr< fragment_shader > fragment;
			constants::config constants;

			size_t hash() const;

			ptrdiff_t compare( const config& ) const;
		};

		virtual const config& cfg() const = 0;

		virtual const igpu::constant_parameters& constant_parameters()
			const = 0;

		virtual const igpu::vertex_parameters& vertex_parameters() const = 0;

		virtual const parameters& batch_parameters() const = 0;

		virtual const parameters& material_parameters() const = 0;

		virtual const parameters& instance_parameters() const = 0;

		virtual ~program() = default;

	protected:
		program() = default;

	private:
		program( const program& ) = delete;
		program& operator=( const program& ) = delete;
	};
}
