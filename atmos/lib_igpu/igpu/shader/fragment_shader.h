
#pragma once

#include "igpu/shader/shader.h"

namespace igpu
{
	class fragment_shader : public shader
	{
	public:
		virtual ~fragment_shader() = default;
	};
}
