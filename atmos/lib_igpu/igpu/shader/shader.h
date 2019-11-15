
#pragma once

#include "igpu/buffer/buffer.h"

namespace igpu
{
	class shader : public buffer
	{
	public:
		struct config
		{};

		virtual ~shader() = default;
	};
}