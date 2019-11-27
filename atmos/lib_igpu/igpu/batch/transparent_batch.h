
#pragma once

#include "igpu/batch/raster_batch.h"

namespace igpu
{
	class transparent_batch : public raster_batch
	{
	public:
		struct config : raster_batch::config
		{};

		virtual ~transparent_batch() = default;
	};
}
