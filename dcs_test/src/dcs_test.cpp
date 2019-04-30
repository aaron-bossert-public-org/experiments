
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
{
}

dcs_test::~dcs_test()
{
}

bool dcs_test::advance()
{
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