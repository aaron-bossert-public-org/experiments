
#pragma once

#include <igpu/buffer/index_format.h>

#include <framework/logging/log.h>

using namespace igpu;

namespace igpu
{
	size_t bytes_per_index(index_format format)
	{
		if (!is_valid(format))
		{
			LOG_CONTEXT(
				CRITICAL,
				"invalid format:%d",
				(int)format);
		}
		else
		{
			switch (format)
			{
			case index_format::UNSIGNED_INT:
				return 4;
			case index_format::UNSIGNED_SHORT:
				return 2;
			}

			LOG_CONTEXT(
				CRITICAL,
				"unhandled format(%s)",
				to_string(format).data());
		}

		return 1;
	}
}
