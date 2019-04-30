#pragma once

#include <chrono>

namespace app
{
	class time
	{
	public:
		using duration = std::chrono::duration<double>;

		virtual ~time() {}

		void advance();
		void play_time(duration t);
		duration play_time() const;
		void frame_time(duration t);
		std::chrono::duration<double> frame_time() const;


	private:

		std::chrono::system_clock::time_point _start_point;
		duration _play_time = duration::zero();
		duration _frame_time = duration::zero();
	};
}
