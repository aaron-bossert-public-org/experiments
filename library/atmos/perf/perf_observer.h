
#pragma once
#include <type_traits>
#include <atmos/partial.h>
#include <chrono>

namespace perf
{
	// So long as this is alive, perf info will be gathered and relayed back to client layer.
	class perf_observer : platform::partial <perf_observer>
	{
	public:
		using duration_t = std::chrono::duration<double>;
		~perf_observer();

		duration_t frame_time() const;
	};
}