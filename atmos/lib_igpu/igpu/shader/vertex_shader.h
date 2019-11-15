
#pragma once

#include "igpu/shader/shader.h"

namespace igpu
{
	class vertex_shader : public shader
	{
	public:
		struct config : shader::config
		{};

		virtual ~vertex_shader() = default;
	};
}
