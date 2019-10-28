
#pragma once

#include <framework/meta_programming/va_macro.h>
#include <igpu/buffer/components.h>

#include <string>

namespace igpu
{
	class vertex_parameter
	{
	public:

		struct config
		{
			std::string name;
			components components;
		};

		virtual const config& cfg() const = 0;

		virtual ~vertex_parameter() = default;

	protected:

		vertex_parameter() = default;

		vertex_parameter(const vertex_parameter&) = delete;
		vertex_parameter& operator= (const vertex_parameter&) = delete;
	};
}
