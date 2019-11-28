
#pragma once

#include "igpu/shader/shader.h"

namespace igpu
{
	class compute_shader : public shader
	{
	public:
		struct config : shader::config
		{};

		virtual ~compute_shader() = default;
	};
}
