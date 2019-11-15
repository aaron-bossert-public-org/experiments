
#pragma once

#include "igpu/texture/color_format.h"
#include "igpu/texture/render_buffer.h"
#include "igpu/texture/sampler.h"

#include "glm/vec2.hpp"

namespace igpu
{
	class render_texture2d : public render_buffer
	{
	public:
		struct config : render_buffer::config
		{};

		virtual ~render_texture2d() = default;
	};
}
