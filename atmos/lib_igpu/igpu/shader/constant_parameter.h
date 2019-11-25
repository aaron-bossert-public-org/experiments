
#pragma once

#include "igpu/shader/constant.h"
#include "igpu/shader/shader_stages.h"

#include <string>

namespace igpu
{
	class constant_parameter
	{
	public:
		struct config
		{
			constant constant;
			size_t binding = ~0U;
			shader_stages shader_stages = shader_stages::NONE;
		};
	};

}