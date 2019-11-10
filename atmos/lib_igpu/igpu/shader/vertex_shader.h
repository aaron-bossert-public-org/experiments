
#pragma once

#include "igpu/shader/shader.h"

namespace igpu
{
	class vertex_shader : public shader
	{
	public:
		virtual ~vertex_shader() = default;
	};
}
