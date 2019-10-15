
#pragma once

#include <framework/meta_programming/va_macro.h>

#include <glm/glm.hpp>
#include <memory>
#include <string_view>
#include <functional>

namespace igpu
{
	class window
	{
	public:

		ENUM_SERIALIZABLE(

			status, DEFAULT(NORMAL),

			(NORMAL, 0),
			(CLOSED, 1)
		);

		virtual ~window() {}

		virtual glm::ivec2 res() const = 0;
		
	protected:

		window() = default;
		window(const window&) = delete;
		window& operator= (const window&) = delete;
	};
}