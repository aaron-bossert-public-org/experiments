
#pragma once

#include "igpu/batch/batch.h"

namespace igpu
{
	class transparent_batch : public batch
	{
	public:
		virtual ~transparent_batch() = default;
	};
}
