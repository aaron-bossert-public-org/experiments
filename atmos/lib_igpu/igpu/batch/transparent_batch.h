
#pragma once

#include <igpu/batch/batch.h>
#include <igpu/batch/batch_binding.h>

namespace igpu
{
	class transparent_batch : public batch
	{
	protected:
		using batch::batch;
	};
}
