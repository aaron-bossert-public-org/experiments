#include "time.h"

using namespace std::chrono;
using namespace app;

void time::advance()
{
	auto prev = _play_time;
	_play_time = system_clock::now() - _start_point;
	_frame_time = _play_time - prev;
}

duration<double> time::play_time() const
{
	return _play_time;
}

void time::play_time(std::chrono::duration<double> t)
{
	_play_time = t;
}

duration<double> time::frame_time() const
{
	return _frame_time;
}

void time::frame_time(std::chrono::duration<double> t)
{
	_frame_time = t;
}

