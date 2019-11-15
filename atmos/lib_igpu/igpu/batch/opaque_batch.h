
#pragma once

#include "igpu/batch/batch.h"


namespace igpu
{
	class opaque_batch : public batch
	{
	public:
		struct config : batch::config
		{};

		virtual ~opaque_batch() = default;
	};
}
