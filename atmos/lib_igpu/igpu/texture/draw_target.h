
#pragma once

#include "glm/vec2.hpp"
#include <memory>

namespace igpu
{
	class render_buffer;
	class depth_buffer;

	class draw_target
	{
	public:
		struct config
		{
			std::shared_ptr< render_buffer > color;
			std::shared_ptr< depth_buffer > depth;
		};

		virtual const render_buffer& color() const = 0;

		virtual const depth_buffer& depth() const = 0;

		glm::ivec2 res() const;

		virtual ~draw_target() = default;
	};
}
