
#pragma once
#include <atmos/partial.h>

namespace perf {
	class perf_observer;

	class perf_observer_vulkan
	{
	public:

	protected:
	};
}

namespace platform
{
	template<>
	class partial<perf::perf_observer> : public perf::perf_observer_vulkan
	{
	};
}

#include <atmos/perf/perf_observer.h>