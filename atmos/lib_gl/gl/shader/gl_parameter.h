
#pragma once

#include "framework/meta_programming/va_macro.h"
#include "igpu/shader/parameter.h"

#include <optional>

namespace igpu
{
	class gl_parameter : public parameter
	{
	public:

		gl_parameter();

		gl_parameter(
			const config&,
			size_t binding,
			int location,
			unsigned binding_index);

		gl_parameter(const gl_parameter&) = default;

		gl_parameter& operator= (const gl_parameter&) = default;

		int location() const;

		unsigned binding_index() const;

	private:

		int _location = -1;
		unsigned _binding_index = 0;
	};
}
