
#pragma once

#include "glm/vec2.hpp"
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

		virtual bool poll_events() const = 0;

		virtual ~window() = default;
	};
}