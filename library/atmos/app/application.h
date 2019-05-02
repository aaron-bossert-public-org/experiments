#pragma once
#include <atmos/partial.h>
#include <memory>
#include <atmos/perf/perf_observer.h>

namespace app
{
	class application : platform::partial <application>
	{
	public:

		static std::unique_ptr<application> make();
		~application();
		bool present();
		std::shared_ptr<perf::perf_observer> make_perf_observer();
	};
}
