
#pragma once

#include "igpu/batch/raster_batch.h"


namespace igpu
{
	class opaque_batch : public raster_batch
	{
	public:
		struct config : raster_batch::config
		{};

		virtual ~opaque_batch() = default;
	};
}
