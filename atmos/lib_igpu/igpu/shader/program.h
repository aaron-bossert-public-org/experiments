
#pragma once

#include "igpu/shader/primitive.h"

#include <unordered_map>

namespace igpu
{
	class fragment_shader;
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

			static size_t hash( const config& );

			static ptrdiff_t compare( const config&, const config& );
		};

		virtual const config& cfg() const = 0;

		virtual const parameters& batch_parameters() const = 0;

		virtual const parameters& material_parameters() const = 0;

		virtual const parameters& instance_parameters() const = 0;

		virtual const igpu::vertex_parameters& vertex_parameters() const = 0;

		virtual ~program() = default;

	protected:
		program() = default;

	private:
		program( const program& ) = delete;
		program& operator=( const program& ) = delete;
	};
}
