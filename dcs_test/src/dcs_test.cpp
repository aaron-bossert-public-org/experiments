
#include <dcs_test/include/dcs_test.h>

#include <framework/logging/log.h>
#include <framework_tests/main/tests.h>
#include <igpu/context/context.h>
#include <igpu/window/window.h>

#include <iostream>


std::unique_ptr<dcs_test> dcs_test::make(const config& cfg)
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

	if (!cfg.context)
	{
		LOG_CONTEXT(CRITICAL, "context is null");
	}
	else
	{
		return std::unique_ptr<dcs_test>(
			new dcs_test(
				cfg));
	}

	return nullptr;
}

void test_init_vulkan_context(const std::shared_ptr < igpu::vulkan_context >& context);
bool test_loop_vulkan_context();

dcs_test::dcs_test(
	const config& cfg)
	: _cfg(cfg)
{
	test_init_vulkan_context(_cfg.context);
}

dcs_test::~dcs_test()
{
}

bool dcs_test::advance()
{
	update();

	handle_input();

	render();

	return test_loop_vulkan_context();
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