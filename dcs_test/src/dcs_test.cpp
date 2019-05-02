
#include "dcs_test.h"
#include <atmos/tests.h>
#include <iostream>

std::unique_ptr<dcs_test> dcs_test::make()
{
	auto results = tests::run();
	if (results.success())
	{
		std::cout << "tests successful" << std::endl;
	}
	else
	{
		std::cout << results.failures() << std::endl;
		throw std::runtime_error(results.failures());
	}

	return std::unique_ptr<dcs_test>(new dcs_test());
}

dcs_test::dcs_test()
	: _application { app::application::make() }
	, _app_perf_observer { _application->make_perf_observer() }
{
}

dcs_test::~dcs_test()
{
}

bool dcs_test::advance()
{
	std::cout << "Frame Time(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(_app_perf_observer->frame_time()).count() << std::endl;

	_time.advance();

	update();

	handle_input();

	render();

	return _application->present();
}

void dcs_test::update()
{

}

void dcs_test::handle_input()
{

}

void dcs_test::render()
{

}