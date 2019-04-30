#pragma once
#include <atmos/partial.h>

namespace app
{
	class application : platform::partial <application>
	{
	public:

		static std::unique_ptr<application> make();
		~application();
		bool present();
	};
}
