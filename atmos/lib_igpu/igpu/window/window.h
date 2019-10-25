
#pragma once

#include <framework/meta_programming/va_macro.h>

#include <glm/glm.hpp>
#include <memory>

namespace igpu
{
	class window
	{
	public:

		struct config
		{
			std::string name;
		};

		virtual const config& cfg() const = 0;

		virtual glm::ivec2 res() const = 0;

		virtual ~window() = default;

	protected:

		window() = default;
		window(const window&) = delete;
		window& operator= (const window&) = delete;
	};
}