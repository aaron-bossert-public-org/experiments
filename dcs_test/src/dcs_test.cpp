
#include <dcs_test/include/dcs_test.h>

#include <framework/logging/log.h>
#include <framework_tests/main/tests.h>
#include <igpu/context/context.h>
#include <igpu/window/window.h>

#include <iostream>


std::unique_ptr<dcs_test> dcs_test::make(std::unique_ptr<igpu::context> context)
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

	if (!context)
	{
		LOG_CONTEXT(CRITICAL, "context is null");
	}
	else
	{
		return std::unique_ptr<dcs_test>(
			new dcs_test(
				std::move(context));
	}

	return nullptr;
}

dcs_test::dcs_test(
	std::unique_ptr<igpu::context> context,
	: _context { std::move(context) }
{
}

dcs_test::~dcs_test()
{
}

bool dcs_test::advance()
{
	update();

	handle_input();

	render();

	return _window->present() != igpu::window::status::CLOSED;
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