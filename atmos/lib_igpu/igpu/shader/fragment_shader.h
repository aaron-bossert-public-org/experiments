
#pragma once

#include "igpu/shader/shader.h"

namespace igpu
{
	class fragment_shader : public shader
	{
	public:
		struct config : shader::config
		{};

		virtual ~fragment_shader() = default;
	};
}
